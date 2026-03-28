# Sonotide

Sonotide is a standalone Windows-only C++20 library that wraps WASAPI behind a production-oriented API for device discovery, render streaming, microphone capture, and loopback capture.

The repository is intentionally structured as an independent package under `third_party/sonotide`, with its own CMake project, install rules, examples, tests, and design documents. The goal is to isolate COM, endpoint activation, format negotiation, and event-driven buffer handling behind a predictable runtime/stream model.

## Current Scope

- Shared-mode event-driven render streams
- Shared-mode event-driven capture streams
- Shared-mode event-driven loopback capture streams
- High-level playback sessions with `load / play / pause / seek / volume`
- Built-in 10-band playback equalizer with presets, custom band gains, and headroom compensation
- Playback-state snapshots with negotiated format and active output-device metadata
- Device enumeration and default-device resolution
- Explicit error model based on `sonotide::result<T>`
- Threaded stream lifecycle with deterministic `start / stop / reset / close`
- Windows-specific internals isolated under `src/internal/win/`

## Repository Layout

- `include/sonotide/`: public API
- `src/`: package implementation and internal runtime backends
- `examples/`: minimal integration programs
- `tests/`: unit tests for platform-neutral components
- `docs/`: architecture, API, mapping, decisions, and migration materials
- `cmake/`: package config templates

## Build

Recommended on Windows and WSL:

```bash
"/mnt/c/Program Files/Microsoft Visual Studio/18/Professional/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe" --preset msvc-x64-debug
"/mnt/c/Program Files/Microsoft Visual Studio/18/Professional/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/cmake.exe" --build --preset msvc-x64-debug
"/mnt/c/Program Files/Microsoft Visual Studio/18/Professional/Common7/IDE/CommonExtensions/Microsoft/CMake/CMake/bin/ctest.exe" --preset msvc-x64-debug
```

If `cmake` is already in `PATH`, the same workflow is:

```bash
cmake --preset msvc-x64-debug
cmake --build --preset msvc-x64-debug
ctest --preset msvc-x64-debug
```

If you run `ctest` manually against a Visual Studio build tree, remember that it is a multi-config generator and requires the configuration name:

```bash
ctest --test-dir build/msvc-x64-debug -C Debug
```

`Sonotide` is Windows-only at runtime. On non-Windows platforms the package still configures, but its stub backend returns `unsupported_platform`.

Playback session support uses Media Foundation for source loading and decoding on Windows.
Playback EQ runs in the render pipeline on decoded float PCM before conversion to the negotiated device format.
Playback sessions also expose both the preferred output device id and the currently active endpoint metadata, which is useful when the system default device changes or recovery rebinds the stream.

## Quick Example

```cpp
#include "sonotide/runtime.h"

class silence_callback final : public sonotide::render_callback {
public:
    sonotide::result<void> on_render(
        sonotide::audio_buffer_view buffer,
        sonotide::stream_timestamp) override {
        std::fill(buffer.bytes.begin(), buffer.bytes.end(), std::byte{0});
        return sonotide::result<void>::success();
    }
};

int main() {
    auto runtime_result = sonotide::runtime::create();
    if (!runtime_result) {
        return 1;
    }

    sonotide::runtime audio = std::move(runtime_result.value());
    silence_callback callback;

    auto stream_result = audio.open_render_stream({}, callback);
    if (!stream_result) {
        return 1;
    }

    auto start_result = stream_result.value().start();
    return start_result ? 0 : 1;
}
```

## Design Priorities

- Clean public API with no raw COM exposure
- Clear ownership and explicit lifecycle
- No god-objects and no helper dumps
- Predictable callback threading and shutdown semantics
- Direct mapping to real WASAPI constraints where it matters

See [docs/foundation.md](docs/foundation.md), [docs/architecture.md](docs/architecture.md), and [docs/api.md](docs/api.md) for the full engineering package.
