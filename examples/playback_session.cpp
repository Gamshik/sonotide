#include <chrono>
#include <iostream>
#include <thread>

#include "sonotide/runtime.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: sonotide_playback_session <uri>\n";
        return 1;
    }

    auto runtime_result = sonotide::runtime::create();
    if (!runtime_result) {
        std::cerr << runtime_result.error().message << '\n';
        return 1;
    }

    sonotide::runtime audio_runtime = std::move(runtime_result.value());
    auto session_result = audio_runtime.open_playback_session();
    if (!session_result) {
        std::cerr << session_result.error().message << '\n';
        return 1;
    }

    auto load_result = session_result.value().load(argv[1]);
    if (!load_result) {
        std::cerr << load_result.error().message << '\n';
        return 1;
    }
    const auto started_at = std::chrono::steady_clock::now();
    while (true) {
        const auto state = session_result.value().state();
        std::cout << "status=" << static_cast<int>(state.status)
                  << " position_ms=" << state.position_ms
                  << " duration_ms=" << state.duration_ms << '\n'
                  << std::flush;
        if (state.status == sonotide::playback_status::idle ||
            state.status == sonotide::playback_status::error) {
            break;
        }

        if (std::chrono::steady_clock::now() - started_at > std::chrono::seconds(30)) {
            std::cerr << "Playback example timed out.\n";
            return 2;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    return 0;
}
