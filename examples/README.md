# Examples

`list_devices.cpp`

- shows device enumeration and default-flag metadata
- useful as a quick smoke test for endpoint discovery

`render_silence.cpp`

- demonstrates runtime creation, render stream opening, and the render callback contract
- useful for validating the shared-mode event-driven render path

`playback_session.cpp`

- demonstrates the high-level playback session API on top of the render engine
- useful for validating Media Foundation source loading, transport, timeline updates, and playback-session level DSP hooks
