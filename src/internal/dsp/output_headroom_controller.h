#pragma once

#include <array>

#include "sonotide/equalizer.h"

namespace sonotide::detail::dsp {

class output_headroom_controller {
public:
    [[nodiscard]] float compute_target_preamp_db(
        const std::array<float, equalizer_band_count>& band_gains_db,
        float sample_rate) const;
};

}  // namespace sonotide::detail::dsp
