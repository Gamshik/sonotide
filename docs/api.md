# API

## Entry Point

`sonotide::runtime` is the package entry point.

Key methods:

- `runtime::create()`
- `runtime::enumerate_devices(device_direction)`
- `runtime::default_device(device_direction, device_role)`
- `runtime::open_render_stream(config, callback)`
- `runtime::open_capture_stream(config, callback)`
- `runtime::open_loopback_stream(config, callback)`

## Device Model

- `device_direction`: `render`, `capture`
- `device_role`: `console`, `multimedia`, `communications`
- `device_selector`: default-by-role or explicit endpoint id
- `device_info`: endpoint id, friendly name, state, default flags

## Format Model

- `format_request`: preferred sample type/rate/channel count
- `audio_format`: negotiated concrete format
- buffers are exposed as interleaved byte spans paired with format metadata

## Stream Lifecycle

- open
- start
- stop
- reset
- close

`status()` provides:

- current state
- requested format
- negotiated format
- callback/frame counters
- device-loss flag

## Callback Rules

Render:

- implement `render_callback::on_render()`
- fill the provided endpoint buffer

Capture:

- implement `capture_callback::on_capture()`
- consume the provided immutable packet bytes

All callbacks:

- execute on the stream worker thread
- must be fast
- may signal failure via `result<void>::failure(...)`

