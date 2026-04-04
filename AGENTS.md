# AGENTS.md

This file is a fast-start guide for AI agents working in the `sonotide` repository.
Read it before making changes so you do not have to reconstruct the project from
scratch.

## 1. Project summary

Sonotide is an independent C++20 audio framework for Windows.

Its main goals are:

- expose a small public API for device enumeration and stream management;
- wrap WASAPI shared-mode rendering, capture, and loopback;
- provide a higher-level playback session built on Media Foundation;
- keep error handling explicit through `sonotide::result<T>`;
- stay reusable as a standalone framework, not as app-specific code.

The repository is intentionally framework-focused: it contains the library,
examples, unit tests, and framework-level documentation. The main production
target is Windows. Non-Windows builds are supported only through a stub backend
that returns `unsupported_platform`.

## 2. Repository layout

- `include/sonotide/`
  Public headers. Treat this as the stable API surface.
- `src/`
  Core implementation for runtime, streams, playback, error model, and DSP.
- `src/internal/win/`
  Windows-specific backend code: WASAPI, Media Foundation, COM helpers, format
  conversion helpers, HRESULT mapping.
- `src/internal/stub/`
  Non-Windows fallback backend that keeps the project buildable outside Windows.
- `src/internal/dsp/`
  Internal EQ and signal-processing pieces used by `playback_session`.
- `examples/`
  Small runnable programs that exercise typical scenarios.
- `tests/`
  Platform-neutral unit tests.
- `docs/`
  Framework documentation inside this repository.
- `.github/workflows/framework-ci.yml`
  CI definition used on GitHub Actions.

Important workspace note:

- there is also a sibling `docs/` repository one level above this repository in
  the wider workspace;
- this file describes the framework repository at `sonotide/`, not the separate
  Mintlify docs site repository.

## 3. Main concepts and entry points

### Public entry points

- `sonotide::runtime`
  Creates the runtime, enumerates devices, opens streams, and acts as the main
  entry point into the backend.
- `sonotide::render_stream`
  RAII wrapper around an internal stream handle for audio output.
- `sonotide::capture_stream`
  RAII wrapper for shared-mode capture streams.
- `sonotide::loopback_capture_stream`
  RAII wrapper for shared-mode loopback capture.
- `sonotide::playback_session`
  Higher-level playback object that loads a source, manages transport state,
  and applies the built-in equalizer.
- `sonotide::result<T>`
  Explicit success/error carrier used across the API instead of exceptions in
  normal control flow.

### Internal split

- the public layer should stay free from raw COM and Windows implementation
  details;
- backend-specific code belongs in `src/internal/win/`;
- platform-neutral orchestration belongs in `src/` and `src/internal/`;
- the stub backend should remain buildable on non-Windows hosts.

## 4. Equalizer model

The equalizer is no longer a fixed 10-slot array with a static band layout.

Current behavior:

- the EQ supports a dynamic number of bands from `0` to `10`;
- users may create and remove bands at runtime;
- users may change both gain and center frequency at runtime;
- helper APIs expose supported band-count limits and supported frequency limits;
- helper APIs can return default band layouts for `0..10` bands;
- editing a band frequency is constrained so band order remains valid.

Implementation notes:

- public EQ types live in `include/sonotide/equalizer.h`;
- runtime EQ control is exposed through `playback_session`;
- DSP execution lives in `src/internal/dsp/equalizer_chain.*`;
- the current implementation uses peaking biquad filters, so each band affects
  a frequency range around its center frequency, not a single mathematical
  point.

If you change EQ behavior, review:

- public API in `include/sonotide/equalizer.h`;
- playback-session integration in `include/sonotide/playback_session.h` and
  `src/playback_session.cpp`;
- DSP internals in `src/internal/dsp/`;
- tests in `tests/unit/equalizer_tests.cpp` and `tests/unit/dsp_tests.cpp`;
- docs and examples that describe EQ behavior.

## 5. Platform and backend expectations

- Windows is the real runtime target.
- WASAPI is used for shared-mode render/capture/loopback streams.
- Media Foundation is used for source decoding in playback scenarios.
- On non-Windows hosts the project should still configure and build by using the
  stub backend.
- Do not leak raw COM types into public headers.
- Do not make the public API depend directly on Media Foundation or WASAPI
  headers.

When changing Windows code, keep the public API portable even if the actual
implementation is Windows-only.

## 6. Build, test, and verification

Always verify changes before considering the task complete.

Canonical commands for this repository are defined in `README.md` and the CMake
presets.

### Main local preset

Recommended path for Windows and WSL:

```bash
cmake --preset msvc-x64-debug
cmake --build --preset msvc-x64-debug
ctest --preset msvc-x64-debug
```

In this workspace, `cmake` may also be invoked through the Visual Studio CMake
binary path if it is not in `PATH`.

### CI preset

GitHub Actions uses:

```bash
cmake --preset ci-ninja-debug
cmake --build --preset ci-ninja-debug
ctest --preset ci-ninja-debug
```

### Build toggles

Defined in `CMakeLists.txt`:

- `SONOTIDE_BUILD_EXAMPLES=ON` by default
- `SONOTIDE_BUILD_TESTS=ON` by default

There is currently no dedicated lint step and no separate typecheck step.
If you search for verification commands and do not find lint/typecheck tasks,
say so explicitly instead of inventing them.

## 7. Existing tests

The test suite is intentionally small and focused on platform-neutral logic.

Current unit-test executables:

- `sonotide_result_tests`
  Result/error model behavior.
- `sonotide_state_machine_tests`
  Internal state-machine transitions.
- `sonotide_equalizer_tests`
  Public equalizer helpers, limits, defaults, and validation.
- `sonotide_dsp_tests`
  DSP primitives such as smoothing, biquad coefficients, headroom logic, and
  equalizer-chain behavior.
- `sonotide_stream_wrapper_tests`
  Public stream wrappers and forwarding/error behavior around stream handles.

Test guidance:

- when changing public API behavior, update the matching unit tests;
- when changing internal DSP behavior, cover success paths, error paths, and
  boundary conditions;
- when adding platform-neutral logic, prefer unit tests in `tests/unit/`;
- when changing Windows-only behavior, unit-test the portable part if possible
  and document any gaps that require real-device or Windows integration testing.

## 8. Examples

Examples are not filler. They act as runnable smoke scenarios and usage
references.

Current examples:

- `examples/list_devices.cpp`
  Device enumeration smoke test.
- `examples/render_silence.cpp`
  Minimal render-path smoke test.
- `examples/playback_session.cpp`
  Higher-level playback-session example.

If you change the API shape or user workflow, check whether one of the examples
should be updated as well.

## 9. Documentation responsibilities

There are two documentation surfaces to keep in mind:

- `sonotide/docs/`
  Framework documentation that ships with this repository.
- sibling workspace repository `../docs`
  Separate Mintlify site for end-user documentation.

If you change API behavior, playback semantics, equalizer behavior, or user
workflow, review both:

- the in-repo docs in `sonotide/docs/`;
- the external docs site repository when it is part of the same workspace.

Do not assume docs are correct after a behavior change. Recheck them.

## 10. Coding expectations

- Prefer keeping public headers concise and implementation details private.
- Preserve RAII lifecycle semantics for streams and sessions.
- Keep errors explicit through `result<T>` and `error`.
- Avoid introducing exceptions into hot-path runtime logic.
- Do not hide important lifecycle failures behind silent fallbacks.
- Keep platform-neutral code portable even when authored from WSL.
- Preserve the separation between public API, internal abstractions, and
  Windows-specific backend code.

For API changes, think through:

- source compatibility of headers;
- test coverage;
- examples;
- framework docs;
- external docs site if applicable.

## 11. Working in WSL

This repository is often edited from WSL while targeting MSVC/Windows builds.

Practical implications:

- the recommended preset is still `msvc-x64-debug`;
- the build may use Windows `cmake.exe` and `ctest.exe` from Visual Studio;
- MSBuild may emit UNC-path warnings from WSL paths even when the build passes;
- GitHub Actions is the most reliable clean Windows validation path.

If a command cannot be run because the host does not have the required Visual
Studio or CMake tooling, say exactly what was attempted and what blocked it.

## 12. CI overview

The repository already has a Windows CI workflow:

- file: `.github/workflows/framework-ci.yml`
- runner: `windows-latest`
- toolchain bootstrap: `ilammy/msvc-dev-cmd@v1`
- verification path: configure, build, and test via `ci-ninja-debug`

When modifying build logic, preset names, test registration, or example/test
targets, review the workflow too.

## 13. Safe first-read checklist for new agents

If you are new to the repository, start in this order:

1. Read `README.md`.
2. Read `CMakePresets.json`.
3. Inspect `CMakeLists.txt` and `tests/CMakeLists.txt`.
4. Inspect the relevant public header in `include/sonotide/`.
5. Inspect the matching implementation file in `src/`.
6. Read the relevant unit tests in `tests/unit/`.
7. If behavior is user-facing, inspect examples and docs before changing it.

Following that sequence is usually enough to make a safe, non-destructive change.
