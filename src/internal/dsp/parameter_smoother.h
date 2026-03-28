#pragma once

#include <cstddef>

namespace sonotide::detail::dsp {

class parameter_smoother {
public:
    parameter_smoother() = default;

    void reset(float value);
    void set_target(float target_value, std::size_t ramp_samples);

    [[nodiscard]] float advance(std::size_t consumed_samples);
    [[nodiscard]] float current_value() const;
    [[nodiscard]] float target_value() const;

private:
    float current_value_ = 0.0F;
    float target_value_ = 0.0F;
    std::size_t remaining_samples_ = 0;
};

}  // namespace sonotide::detail::dsp
