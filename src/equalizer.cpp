#include "sonotide/equalizer.h"

#include <algorithm>
#include <array>
#include <cmath>

#include "internal/dsp/equalizer_response_sampler.h"
#include "internal/dsp/output_headroom_controller.h"

namespace sonotide {
namespace {

constexpr std::array<float, equalizer_max_band_count> kReferenceBandFrequenciesHz{
    60.0F,
    170.0F,
    310.0F,
    600.0F,
    1000.0F,
    3000.0F,
    6000.0F,
    12000.0F,
    14000.0F,
    16000.0F,
};

float interpolate_default_band_frequency(
    const std::size_t band_count,
    const std::size_t band_index) {
    if (band_count == 1U) {
        return kReferenceBandFrequenciesHz[kReferenceBandFrequenciesHz.size() / 2U];
    }

    const float reference_position =
        static_cast<float>(band_index) *
        static_cast<float>(kReferenceBandFrequenciesHz.size() - 1U) /
        static_cast<float>(band_count - 1U);
    const std::size_t lower_index = static_cast<std::size_t>(reference_position);
    const std::size_t upper_index = (std::min)(
        lower_index + 1U,
        kReferenceBandFrequenciesHz.size() - 1U);
    if (lower_index == upper_index) {
        return kReferenceBandFrequenciesHz[lower_index];
    }

    const float lower_frequency_hz = kReferenceBandFrequenciesHz[lower_index];
    const float upper_frequency_hz = kReferenceBandFrequenciesHz[upper_index];
    const float interpolation = reference_position - static_cast<float>(lower_index);

    const float lower_log_frequency = std::log(lower_frequency_hz);
    const float upper_log_frequency = std::log(upper_frequency_hz);
    return std::exp(
        lower_log_frequency +
        (upper_log_frequency - lower_log_frequency) * interpolation);
}

float clamp_equalizer_gain_db(const float gain_db) {
    return (std::clamp)(gain_db, -12.0F, 12.0F);
}

float clamp_equalizer_q_value(const float q_value) {
    const equalizer_q_limits q_limits = supported_equalizer_q_limits();
    return (std::clamp)(q_value, q_limits.min_q_value, q_limits.max_q_value);
}

std::vector<equalizer_band> sanitize_equalizer_bands(std::span<const equalizer_band> bands) {
    const equalizer_frequency_limits frequency_limits = supported_equalizer_frequency_limits();

    std::vector<equalizer_band> sanitized_bands;
    sanitized_bands.reserve((std::min)(bands.size(), static_cast<std::size_t>(equalizer_max_band_count)));

    for (const equalizer_band& band : bands) {
        if (sanitized_bands.size() == equalizer_max_band_count) {
            break;
        }

        sanitized_bands.push_back(equalizer_band{
            .center_frequency_hz = (std::clamp)(
                band.center_frequency_hz,
                frequency_limits.min_frequency_hz,
                frequency_limits.max_frequency_hz),
            .gain_db = clamp_equalizer_gain_db(band.gain_db),
            .q_value = clamp_equalizer_q_value(band.q_value),
        });
    }

    return sanitized_bands;
}

}  // namespace

equalizer_band_count_limits supported_equalizer_band_count_limits() noexcept {
    return {};
}

equalizer_frequency_limits supported_equalizer_frequency_limits() noexcept {
    return {};
}

equalizer_q_limits supported_equalizer_q_limits() noexcept {
    return {};
}

std::vector<equalizer_band> make_default_equalizer_bands(std::size_t band_count) {
    const equalizer_band_count_limits band_count_limits = supported_equalizer_band_count_limits();
    band_count = (std::clamp)(
        band_count,
        band_count_limits.min_band_count,
        band_count_limits.max_band_count);

    std::vector<equalizer_band> bands;
    bands.reserve(band_count);
    for (std::size_t band_index = 0; band_index < band_count; ++band_index) {
        bands.push_back(equalizer_band{
            .center_frequency_hz = interpolate_default_band_frequency(band_count, band_index),
            .gain_db = 0.0F,
            .q_value = default_equalizer_q_value,
        });
    }

    return bands;
}

std::optional<equalizer_frequency_range> equalizer_band_editable_frequency_range(
    const std::span<const equalizer_band> bands,
    const std::size_t band_index) noexcept {
    if (band_index >= bands.size()) {
        return std::nullopt;
    }

    const equalizer_frequency_limits frequency_limits = supported_equalizer_frequency_limits();
    float min_frequency_hz = frequency_limits.min_frequency_hz;
    float max_frequency_hz = frequency_limits.max_frequency_hz;

    if (band_index > 0U) {
        min_frequency_hz = (std::max)(
            min_frequency_hz,
            bands[band_index - 1U].center_frequency_hz + frequency_limits.min_band_spacing_hz);
    }
    if (band_index + 1U < bands.size()) {
        max_frequency_hz = (std::min)(
            max_frequency_hz,
            bands[band_index + 1U].center_frequency_hz - frequency_limits.min_band_spacing_hz);
    }

    if (min_frequency_hz > max_frequency_hz) {
        return std::nullopt;
    }

    return equalizer_frequency_range{
        .min_frequency_hz = min_frequency_hz,
        .max_frequency_hz = max_frequency_hz,
    };
}

result<equalizer_response_curve> sample_equalizer_response(
    const equalizer_state& state,
    const float sample_rate_hz,
    const std::span<const float> frequencies_hz) {
    if (sample_rate_hz <= 0.0F) {
        error failure;
        failure.category = error_category::configuration;
        failure.code = error_code::invalid_argument;
        failure.operation = "sample_equalizer_response";
        failure.message = "Sample rate must be greater than zero.";
        return result<equalizer_response_curve>::failure(std::move(failure));
    }
    if (frequencies_hz.empty()) {
        error failure;
        failure.category = error_category::configuration;
        failure.code = error_code::invalid_argument;
        failure.operation = "sample_equalizer_response";
        failure.message = "At least one frequency point is required to sample the equalizer response.";
        return result<equalizer_response_curve>::failure(std::move(failure));
    }

    const float nyquist_frequency_hz = sample_rate_hz * 0.5F;
    for (const float frequency_hz : frequencies_hz) {
        if (frequency_hz <= 0.0F || frequency_hz > nyquist_frequency_hz) {
            error failure;
            failure.category = error_category::configuration;
            failure.code = error_code::invalid_argument;
            failure.operation = "sample_equalizer_response";
            failure.message =
                "Requested response frequency must be greater than zero and not exceed Nyquist.";
            return result<equalizer_response_curve>::failure(std::move(failure));
        }
    }

    equalizer_response_curve response_curve;
    response_curve.sample_rate_hz = sample_rate_hz;
    response_curve.enabled = state.enabled;
    response_curve.applied_output_gain_db = state.enabled ? clamp_equalizer_gain_db(state.output_gain_db) : 0.0F;
    response_curve.points.reserve(frequencies_hz.size());

    const std::vector<equalizer_band> sanitized_bands = sanitize_equalizer_bands(state.bands);
    if (state.enabled) {
        detail::dsp::output_headroom_controller headroom_controller;
        response_curve.applied_headroom_compensation_db =
            headroom_controller.compute_target_preamp_db(sanitized_bands, sample_rate_hz);
    }

    for (const float frequency_hz : frequencies_hz) {
        float response_db = 0.0F;
        if (state.enabled) {
            response_db = detail::dsp::sample_equalizer_band_response_db(
                sanitized_bands,
                sample_rate_hz,
                frequency_hz);
            response_db += response_curve.applied_headroom_compensation_db;
            response_db += response_curve.applied_output_gain_db;
        }

        response_curve.points.push_back(equalizer_response_point{
            .frequency_hz = frequency_hz,
            .response_db = response_db,
        });
    }

    return result<equalizer_response_curve>::success(std::move(response_curve));
}

}  // namespace sonotide
