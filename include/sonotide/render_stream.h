#pragma once

#include <memory>

#include "sonotide/result.h"
#include "sonotide/stream_status.h"

namespace sonotide {
class render_stream;
namespace detail {
class stream_handle;
render_stream make_render_stream(std::shared_ptr<stream_handle> handle);
}

class render_stream {
public:
    render_stream() = default;
    ~render_stream() = default;

    render_stream(render_stream&&) noexcept = default;
    render_stream& operator=(render_stream&&) noexcept = default;

    render_stream(const render_stream&) = delete;
    render_stream& operator=(const render_stream&) = delete;

    [[nodiscard]] bool is_open() const noexcept;
    result<void> start();
    result<void> stop();
    result<void> reset();
    result<void> close();
    [[nodiscard]] stream_status status() const;

private:
    explicit render_stream(std::shared_ptr<detail::stream_handle> handle) noexcept;

    std::shared_ptr<detail::stream_handle> handle_;

    friend class runtime;
    friend class playback_session;
    friend render_stream detail::make_render_stream(std::shared_ptr<detail::stream_handle> handle);
};

}  // namespace sonotide
