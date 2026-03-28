#pragma once

#include <cstddef>
#include <vector>

namespace sonotide::detail::dsp {

struct biquad_coefficients {
    float b0 = 1.0F;
    float b1 = 0.0F;
    float b2 = 0.0F;
    float a1 = 0.0F;
    float a2 = 0.0F;
};

class biquad_filter {
public:
    void configure(std::size_t channel_count, biquad_coefficients coefficients);
    void set_coefficients(biquad_coefficients coefficients);
    void reset();
    void process(float* interleaved_samples, std::size_t frame_count, std::size_t channel_count);

private:
    biquad_coefficients coefficients_{};
    std::vector<float> z1_;
    std::vector<float> z2_;
};

[[nodiscard]] biquad_coefficients make_peaking_coefficients(
    float sample_rate,
    float center_frequency_hz,
    float q_value,
    float gain_db);

}  // namespace sonotide::detail::dsp
