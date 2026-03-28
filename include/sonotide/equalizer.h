#pragma once

#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace sonotide {

inline constexpr std::size_t equalizer_band_count = 10;

enum class equalizer_preset_id {
    flat,
    bass_boost,
    treble_boost,
    vocal,
    pop,
    rock,
    electronic,
    hip_hop,
    jazz,
    classical,
    acoustic,
    dance,
    piano,
    spoken_podcast,
    loudness,
    custom,
};

enum class equalizer_status {
    loading,
    ready,
    unsupported_audio_path,
    audio_engine_unavailable,
    error,
};

struct equalizer_band {
    float center_frequency_hz = 0.0F;
    float gain_db = 0.0F;
};

struct equalizer_preset {
    equalizer_preset_id id = equalizer_preset_id::flat;
    const char* title = "Flat";
    std::array<float, equalizer_band_count> gains_db{};
};

struct equalizer_state {
    equalizer_status status = equalizer_status::loading;
    bool enabled = false;
    equalizer_preset_id active_preset_id = equalizer_preset_id::flat;
    std::array<equalizer_band, equalizer_band_count> bands{};
    std::array<float, equalizer_band_count> last_nonflat_band_gains_db{};
    std::vector<equalizer_preset> available_presets;
    float output_gain_db = 0.0F;
    float headroom_compensation_db = 0.0F;
    std::string error_message;
};

[[nodiscard]] inline std::string_view to_string(const equalizer_preset_id preset_id) {
    switch (preset_id) {
        case equalizer_preset_id::flat:
            return "flat";
        case equalizer_preset_id::bass_boost:
            return "bass_boost";
        case equalizer_preset_id::treble_boost:
            return "treble_boost";
        case equalizer_preset_id::vocal:
            return "vocal";
        case equalizer_preset_id::pop:
            return "pop";
        case equalizer_preset_id::rock:
            return "rock";
        case equalizer_preset_id::electronic:
            return "electronic";
        case equalizer_preset_id::hip_hop:
            return "hip_hop";
        case equalizer_preset_id::jazz:
            return "jazz";
        case equalizer_preset_id::classical:
            return "classical";
        case equalizer_preset_id::acoustic:
            return "acoustic";
        case equalizer_preset_id::dance:
            return "dance";
        case equalizer_preset_id::piano:
            return "piano";
        case equalizer_preset_id::spoken_podcast:
            return "spoken_podcast";
        case equalizer_preset_id::loudness:
            return "loudness";
        case equalizer_preset_id::custom:
            return "custom";
    }

    return "custom";
}

[[nodiscard]] inline std::string_view to_string(const equalizer_status status) {
    switch (status) {
        case equalizer_status::loading:
            return "loading";
        case equalizer_status::ready:
            return "ready";
        case equalizer_status::unsupported_audio_path:
            return "unsupported_audio_path";
        case equalizer_status::audio_engine_unavailable:
            return "audio_engine_unavailable";
        case equalizer_status::error:
            return "error";
    }

    return "error";
}

[[nodiscard]] inline std::optional<equalizer_preset_id> equalizer_preset_id_from_string(
    const std::string_view value) {
    if (value == "flat") {
        return equalizer_preset_id::flat;
    }
    if (value == "bass_boost") {
        return equalizer_preset_id::bass_boost;
    }
    if (value == "treble_boost") {
        return equalizer_preset_id::treble_boost;
    }
    if (value == "vocal") {
        return equalizer_preset_id::vocal;
    }
    if (value == "pop") {
        return equalizer_preset_id::pop;
    }
    if (value == "rock") {
        return equalizer_preset_id::rock;
    }
    if (value == "electronic") {
        return equalizer_preset_id::electronic;
    }
    if (value == "hip_hop") {
        return equalizer_preset_id::hip_hop;
    }
    if (value == "jazz") {
        return equalizer_preset_id::jazz;
    }
    if (value == "classical") {
        return equalizer_preset_id::classical;
    }
    if (value == "acoustic") {
        return equalizer_preset_id::acoustic;
    }
    if (value == "dance") {
        return equalizer_preset_id::dance;
    }
    if (value == "piano") {
        return equalizer_preset_id::piano;
    }
    if (value == "spoken_podcast") {
        return equalizer_preset_id::spoken_podcast;
    }
    if (value == "loudness") {
        return equalizer_preset_id::loudness;
    }
    if (value == "custom") {
        return equalizer_preset_id::custom;
    }

    return std::nullopt;
}

}  // namespace sonotide
