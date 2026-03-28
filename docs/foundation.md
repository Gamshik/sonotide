# Sonotide Foundation

## 1. Repository Name Decision

Candidate names:

- `Sonotide`
- `Resonrail`
- `Echoforge`
- `Timberspan`

Final choice: `Sonotide`

Why this one:

- It is audio-linked without sounding toy-like.
- It works as a GitHub repo, a CMake package, and a C++ namespace.
- It is specific enough to feel intentional, but not locked to a single product tier or vendor.
- It fits a Windows audio infrastructure library better than a marketing-heavy brand name.

## 2. Architectural Principles

- Public API stays free of COM and Win32 ownership details.
- Runtime and stream objects are move-only and own their resources explicitly.
- Shared-mode WASAPI streaming is the stable default; unsupported paths fail explicitly rather than pretending to work.
- Internal layering separates device discovery, format negotiation, and stream execution.
- State transitions are explicit and testable.
- Callback hot paths stay narrow and avoid hidden allocations except for silent capture packet synthesis.

Consciously excluded anti-patterns:

- God backend classes with devices, streams, negotiation, and policy mixed together
- `utils.*` dumps
- Public raw `HRESULT` transport
- Ambiguous callback thread ownership
- Implicit start on open
- Silent fallback to a different stream type

## 3. Final Repository Structure

```text
third_party/sonotide/
├── .gitignore
├── CMakeLists.txt
├── README.md
├── cmake/
│   └── SonotideConfig.cmake.in
├── docs/
│   ├── api.md
│   ├── architecture.md
│   ├── foundation.md
│   ├── migration-plan.md
│   ├── wasapi-mapping.md
│   └── design-decisions/
│       ├── 0001-name-and-boundaries.md
│       ├── 0002-threading-and-lifetime.md
│       └── 0003-format-negotiation.md
├── examples/
│   ├── CMakeLists.txt
│   ├── README.md
│   ├── list_devices.cpp
│   └── render_silence.cpp
├── include/
│   └── sonotide/
│       ├── audio_buffer.h
│       ├── audio_format.h
│       ├── capture_stream.h
│       ├── device_info.h
│       ├── device_selector.h
│       ├── error.h
│       ├── loopback_capture_stream.h
│       ├── render_stream.h
│       ├── result.h
│       ├── runtime.h
│       ├── stream_callback.h
│       ├── stream_config.h
│       ├── stream_state.h
│       ├── stream_status.h
│       └── version.h
├── src/
│   ├── capture_stream.cpp
│   ├── error.cpp
│   ├── loopback_capture_stream.cpp
│   ├── render_stream.cpp
│   ├── runtime.cpp
│   └── internal/
│       ├── runtime_backend.h
│       ├── state_machine.cpp
│       ├── state_machine.h
│       ├── stub/
│       │   └── unsupported_runtime_backend.cpp
│       └── win/
│           ├── com_scope.cpp
│           ├── com_scope.h
│           ├── device_utils.cpp
│           ├── device_utils.h
│           ├── hresult_utils.cpp
│           ├── hresult_utils.h
│           ├── wasapi_runtime_backend.cpp
│           ├── wasapi_stream_handle.cpp
│           ├── wasapi_stream_handle.h
│           ├── wave_format_utils.cpp
│           └── wave_format_utils.h
└── tests/
    ├── CMakeLists.txt
    └── unit/
        ├── result_tests.cpp
        └── state_machine_tests.cpp
```

Responsibility split:

- `include/sonotide`: public contract only
- `src/internal/win`: WASAPI, COM, event handles, format negotiation
- `src/internal/stub`: non-Windows safety net
- `docs`: implementation and migration decisions, not generic prose

## 4. Module Breakdown

- `runtime`
  - Entry point for enumeration and stream opening
  - Depends on `detail::runtime_backend`
- `device model`
  - `device_info`, `device_selector`
  - Stable public selectors and metadata
- `format model`
  - `audio_format`, `format_request`
  - Negotiated/requested boundary
- `stream contracts`
  - configs, state, status, callbacks, stream facades
- `win/device_utils`
  - endpoint enumeration, device resolution, default role checks
- `win/wave_format_utils`
  - `WAVEFORMATEX` translation and shared-mode negotiation
- `win/wasapi_stream_handle`
  - worker-thread orchestration for render/capture/loopback
- `internal/state_machine`
  - platform-neutral stream lifecycle validation

Dependency direction:

- Public API depends on nothing internal
- `runtime.cpp` depends on `runtime_backend.h`
- Windows backend depends on public models plus focused internal Win files
- Tests depend only on public API and state machine internals

## 5. Public API Design

Main public types:

- `sonotide::runtime`
- `sonotide::render_stream`
- `sonotide::capture_stream`
- `sonotide::loopback_capture_stream`
- `sonotide::render_callback`
- `sonotide::capture_callback`
- `sonotide::audio_format`
- `sonotide::format_request`
- `sonotide::render_stream_config`
- `sonotide::capture_stream_config`
- `sonotide::loopback_stream_config`
- `sonotide::stream_status`
- `sonotide::error`
- `sonotide::result<T>`

API properties:

- `runtime::create()` constructs the backend boundary explicitly.
- `open_*_stream()` binds config plus callback and returns a move-only stream object.
- `start()` owns the expensive runtime init and blocks until the worker either succeeds or fails deterministically.
- `status()` exposes negotiated format, state, counters, and device-loss flag.

Example:

```cpp
auto runtime_result = sonotide::runtime::create();
if (!runtime_result) {
    return;
}

sonotide::runtime audio = std::move(runtime_result.value());
auto devices = audio.enumerate_devices(sonotide::device_direction::render);
```

## 6. Internal Architecture

Layers:

- Low-level
  - COM init, endpoint lookup, `IAudioClient`, `IAudioRenderClient`, `IAudioCaptureClient`
- Internal orchestration
  - event handles, worker thread startup, start/stop/reset/close semantics, callback dispatch
- Public API
  - `runtime`, stream facades, result/error transport

Ownership:

- `runtime` owns a shared backend object
- public stream objects own shared `stream_handle` implementations
- each running stream owns exactly one worker thread and its Win handles
- callback object lifetime is owned by the integrator and must outlive the stream

Shutdown semantics:

- `stop()` signals a stop event and joins the worker thread
- `reset()` clears negotiated state and counters after stopping
- `close()` is idempotent and terminal
- destructor calls `close()` to avoid orphaned threads

## 7. Stream Model

Stream types:

- render
- capture
- loopback capture

State machine:

- `created` -> `prepared` on open
- `prepared` -> `running` on successful `start()`
- `running` -> `stopped` on `stop()`
- `stopped` -> `prepared` on `reset()`
- any active stream -> `faulted` on runtime failure
- non-closed states -> `closed` on `close()`

Callback model:

- render callback writes directly into the endpoint-facing buffer view
- capture callbacks receive immutable packet views
- callbacks run on the stream worker thread
- callback failures terminate the stream and surface via `on_stream_error`

## 8. Format Model

- `format_request` expresses preferred sample type/rate/channels
- `audio_format` captures the negotiated runtime shape
- shared-mode negotiation prefers the requested format when fully specified
- if requested format is unsupported and fallback is allowed, Sonotide accepts the closest supported format or device mix format
- buffers are interleaved in the current implementation

## 9. Error Model

Error categories:

- configuration
- initialization
- device
- format
- stream
- callback
- platform

Transport model:

- public API returns `result<T>`
- `error` carries category, code, message, operation, native code, and recoverability hint
- `HRESULT` values are preserved in `native_code`

## 10. CMake / Build Layout

- target: `sonotide`
- alias: `Sonotide::sonotide`
- Windows links: `avrt`, `mfplat`, `mfreadwrite`, `mfuuid`, `mmdevapi`, `ole32`
- package config: `SonotideConfig.cmake`
- install rules cover headers, library target, and exported package target set

## 11. Testing Strategy

- unit
  - `result<T>`
  - stream state machine
- integration
  - list devices
  - open/start/stop render stream
  - open/start/stop capture stream
  - open/start/stop loopback stream
- smoke
  - examples compiled in CI
  - Windows-only runtime tests on actual audio-capable runner

## 12. Documentation Layout

- `README.md`: product overview and first-use path
- `docs/architecture.md`: layers, ownership, module boundaries
- `docs/api.md`: public API contract and examples
- `docs/wasapi-mapping.md`: public concepts to raw WASAPI mapping
- `docs/migration-plan.md`: staged extraction from current app backend
- `docs/design-decisions/*`: stable ADR-style decisions

## 13. Migration Plan

Phase 1:

- move device enumeration and default-device selection into Sonotide
- adapt current app code to consume Sonotide device DTOs

Phase 2:

- migrate output streaming to `render_stream`
- keep Media Foundation source decoding in the app temporarily

Phase 3:

- migrate microphone and loopback capture consumers
- add app-side adapters for DSP/EQ layering above Sonotide

Phase 4:

- delete old WASAPI-specific backend pieces from the app
- keep only app-specific decode, transport, and DSP orchestration

## 14. Initial Code Base

The repo now contains a real implementation base:

- working Windows device enumeration
- working shared-mode event-driven render/capture/loopback stream engine
- installable CMake package
- public API headers
- tests for the platform-neutral lifecycle pieces

## 15. README Skeleton

The README has been materialized as a real package entry document, not a placeholder. It already includes:

- what Sonotide is
- repository layout
- build steps
- quick example
- scope and current guarantees
