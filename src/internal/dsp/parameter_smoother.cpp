#include "internal/dsp/parameter_smoother.h"

namespace sonotide::detail::dsp {

void parameter_smoother::reset(const float value) {
    current_value_ = value;
    target_value_ = value;
    remaining_samples_ = 0;
}

void parameter_smoother::set_target(const float target_value, const std::size_t ramp_samples) {
    target_value_ = target_value;
    if (ramp_samples == 0) {
        current_value_ = target_value_;
        remaining_samples_ = 0;
        return;
    }

    remaining_samples_ = ramp_samples;
}

float parameter_smoother::advance(const std::size_t consumed_samples) {
    if (remaining_samples_ == 0) {
        return current_value_;
    }

    if (consumed_samples >= remaining_samples_) {
        current_value_ = target_value_;
        remaining_samples_ = 0;
        return current_value_;
    }

    const float delta = target_value_ - current_value_;
    current_value_ += delta * (
        static_cast<float>(consumed_samples) /
        static_cast<float>(remaining_samples_));
    remaining_samples_ -= consumed_samples;
    return current_value_;
}

float parameter_smoother::current_value() const {
    return current_value_;
}

float parameter_smoother::target_value() const {
    return target_value_;
}

}  // namespace sonotide::detail::dsp
