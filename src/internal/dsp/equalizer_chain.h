#pragma once

#include <array>
#include <cstddef>
#include <vector>

#include "sonotide/equalizer.h"

#include "internal/dsp/biquad_filter.h"
#include "internal/dsp/output_headroom_controller.h"
#include "internal/dsp/parameter_smoother.h"

namespace sonotide::detail::dsp {

class equalizer_chain {
public:
    void configure(float sample_rate, std::size_t channel_count);
    void reset();
    void set_enabled(bool enabled);
    void set_band_gains(const std::array<float, equalizer_band_count>& band_gains_db);
    void set_output_gain_db(float output_gain_db);
    void set_volume_linear(float volume_linear);
    void process(float* interleaved_samples, std::size_t frame_count);

    [[nodiscard]] std::array<float, equalizer_band_count> target_band_gains_db() const;
    [[nodiscard]] float headroom_compensation_db() const;
    [[nodiscard]] bool enabled() const;
    [[nodiscard]] float sample_rate() const;
    [[nodiscard]] std::size_t channel_count() const;
    [[nodiscard]] float output_gain_db() const;
    [[nodiscard]] static const std::array<float, equalizer_band_count>& band_frequencies_hz();

private:
    void update_filter_coefficients(std::size_t control_block_frames);

    float sample_rate_ = 0.0F;
    std::size_t channel_count_ = 0;
    std::array<biquad_filter, equalizer_band_count> filters_;
    std::array<parameter_smoother, equalizer_band_count> band_smoothers_;
    parameter_smoother wet_mix_smoother_;
    parameter_smoother preamp_smoother_;
    parameter_smoother output_gain_smoother_;
    parameter_smoother volume_smoother_;
    std::array<float, equalizer_band_count> current_band_gains_db_{};
    std::array<float, equalizer_band_count> target_band_gains_db_{};
    output_headroom_controller headroom_controller_;
    float output_gain_db_ = 0.0F;
    float headroom_compensation_db_ = 0.0F;
    bool enabled_ = false;
};

[[nodiscard]] std::vector<equalizer_preset> builtin_equalizer_presets();

}  // namespace sonotide::detail::dsp
