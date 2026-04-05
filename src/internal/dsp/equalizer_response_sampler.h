#pragma once

#include <span>

#include "sonotide/equalizer.h"

#include "internal/dsp/biquad_filter.h"

namespace sonotide::detail::dsp {

/// Вычисляет отклик одной biquad-секции в dB на заданной частоте.
[[nodiscard]] float evaluate_frequency_response_db(
    const biquad_coefficients& coefficients,
    float normalized_frequency);

/// Возвращает суммарный отклик всех полос эквалайзера в dB на заданной частоте.
[[nodiscard]] float sample_equalizer_band_response_db(
    std::span<const equalizer_band> bands,
    float sample_rate_hz,
    float frequency_hz);

}  // namespace sonotide::detail::dsp
