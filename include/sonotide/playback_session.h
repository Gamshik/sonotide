#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "sonotide/device_info.h"
#include "sonotide/equalizer.h"
#include "sonotide/playback_state.h"
#include "sonotide/render_stream.h"
#include "sonotide/result.h"
#include "sonotide/stream_config.h"

namespace sonotide {
namespace detail {
class runtime_backend;
}

struct playback_session_config {
    render_stream_config render;
    bool auto_play_on_load = true;
    int initial_volume_percent = 100;
    std::optional<equalizer_state> initial_equalizer_state;
};

class playback_session {
public:
    playback_session() = default;
    ~playback_session();

    playback_session(playback_session&&) noexcept;
    playback_session& operator=(playback_session&&) noexcept;

    playback_session(const playback_session&) = delete;
    playback_session& operator=(const playback_session&) = delete;

    [[nodiscard]] bool is_open() const noexcept;

    result<void> load(std::string source_uri);
    result<void> play();
    result<void> pause();
    result<void> seek_to(std::int64_t position_ms);
    result<void> set_volume_percent(int volume_percent);
    result<void> set_equalizer_enabled(bool enabled);
    result<void> select_equalizer_preset(equalizer_preset_id preset_id);
    result<void> set_equalizer_band_gain(std::size_t band_index, float gain_db);
    result<void> reset_equalizer();
    result<void> set_equalizer_output_gain(float output_gain_db);
    result<void> apply_equalizer_state(const equalizer_state& state);
    result<std::vector<device_info>> list_output_devices() const;
    result<void> select_output_device(std::string device_id);
    [[nodiscard]] playback_state state() const;
    [[nodiscard]] equalizer_state equalizer_state() const;
    result<void> close();

private:
    class implementation;

    explicit playback_session(std::unique_ptr<implementation> implementation) noexcept;
    static result<playback_session> create(
        std::shared_ptr<detail::runtime_backend> backend,
        const playback_session_config& config);

    std::unique_ptr<implementation> implementation_;

    friend class runtime;
};

}  // namespace sonotide
