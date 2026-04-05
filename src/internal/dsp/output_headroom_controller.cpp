#include "internal/dsp/output_headroom_controller.h"

#include <algorithm>
#include <cmath>

#include "internal/dsp/equalizer_response_sampler.h"

namespace sonotide::detail::dsp {
namespace {

}  // namespace

/// Вычисляет консервативное значение предусиления, уменьшающее риск клиппинга.
float output_headroom_controller::compute_target_preamp_db(
    const std::span<const equalizer_band> bands,
    const float sample_rate) const {
    if (sample_rate <= 0.0F || bands.empty()) {
        /// Без валидной частоты дискретизации безопасно оценить отклик полос невозможно.
        return 0.0F;
    }

    /// АЧХ берётся в ограниченном числе точек, чтобы оценка оставалась дешёвой.
    float max_response_db = 0.0F;
    constexpr int frequency_points = 96;

    for (int point_index = 0; point_index < frequency_points; ++point_index) {
        /// Логарифмически распределённые частоты покрывают слышимый диапазон равномернее, чем линейные.
        const float ratio = static_cast<float>(point_index) /
            static_cast<float>(frequency_points - 1);
        const float frequency_hz =
            (std::min)(20.0F * std::pow(1000.0F, ratio), sample_rate * 0.45F);
        const float response_db =
            sample_equalizer_band_response_db(bands, sample_rate, frequency_hz);

        max_response_db = (std::max)(max_response_db, response_db);
    }

    /// Поверх оценённого пикового отклика оставляется небольшой запас безопасности.
    return max_response_db > 0.0F ? -(max_response_db + 1.0F) : 0.0F;
}

}  // пространство имён sonotide::detail::dsp
