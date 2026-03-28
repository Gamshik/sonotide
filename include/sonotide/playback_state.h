#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "sonotide/audio_format.h"

namespace sonotide {

enum class playback_status {
    idle,
    loading,
    playing,
    paused,
    error,
};

struct playback_state {
    playback_status status = playback_status::idle;
    std::string source_uri;
    std::string error_message;
    std::string preferred_output_device_id;
    std::string active_output_device_id;
    std::string active_output_device_name;
    std::int64_t position_ms = 0;
    std::int64_t duration_ms = 0;
    int volume_percent = 100;
    std::uint64_t completion_token = 0;
    std::optional<audio_format> negotiated_format;
    bool active_output_device_is_default = false;
    bool device_lost = false;
};

}  // namespace sonotide
