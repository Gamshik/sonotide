#pragma once

#include <mfidl.h>
#include <mfobjects.h>
#include <mfreadwrite.h>
#include <wrl/client.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "sonotide/audio_format.h"
#include "sonotide/result.h"

namespace sonotide::detail::win {

struct decoded_audio_block {
    std::vector<float> samples;
    std::int64_t position_ms = 0;
    std::int64_t duration_ms = 0;
    bool end_of_stream = false;
};

class media_foundation_decoder {
public:
    media_foundation_decoder() = default;

    [[nodiscard]] result<void> open(const std::string& source_uri, const audio_format& output_format);
    [[nodiscard]] result<void> seek_to(std::int64_t position_ms);
    [[nodiscard]] result<decoded_audio_block> read_frames(std::uint32_t frame_count);

    void close();

    [[nodiscard]] bool is_open() const noexcept;
    [[nodiscard]] const audio_format& output_format() const noexcept;
    [[nodiscard]] std::int64_t duration_ms() const noexcept;

private:
    [[nodiscard]] result<void> ensure_decoded_frames(std::uint32_t frame_count);

    Microsoft::WRL::ComPtr<IMFSourceReader> source_reader_;
    audio_format output_format_{};
    std::vector<float> decoded_samples_;
    std::size_t decoded_sample_offset_ = 0;
    std::int64_t next_sample_time_100ns_ = 0;
    std::int64_t duration_100ns_ = 0;
    bool end_of_stream_ = false;
};

}  // namespace sonotide::detail::win
