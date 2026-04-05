# Examples

The examples are part of the framework workflow, not filler. Each one exercises
one layer of Sonotide and gives you a quick way to validate that the framework
builds and runs in the target environment.

## `sonotide_list_devices.exe`

This example enumerates the available audio devices and prints the default
devices. It is useful when you want to confirm that `runtime` can see the
system audio stack and read device metadata correctly.

Run:

```bat
build\msvc-x64-debug\examples\Debug\sonotide_list_devices.exe
```

## `sonotide_render_silence.exe`

This example opens a render stream and feeds it silence. It may not produce any
visible output, so treat it as a quick smoke test for the render path.

Run:

```bat
build\msvc-x64-debug\examples\Debug\sonotide_render_silence.exe
```

If you want an explicit result, check the exit code:

```bat
build\msvc-x64-debug\examples\Debug\sonotide_render_silence.exe
echo %ERRORLEVEL%
```

## `sonotide_playback_session.exe`

This is the higher-level example. It creates a playback session, loads a source
through Media Foundation, builds the transport layer, and tracks playback
state.

Run:

```bat
build\msvc-x64-debug\examples\Debug\sonotide_playback_session.exe "C:\Windows\Media\notify.wav"
```

From WSL or another bash shell you can invoke the same binaries through
`cmd.exe /c`:

```bash
cmd.exe /c build\\msvc-x64-debug\\examples\\Debug\\sonotide_list_devices.exe
cmd.exe /c build\\msvc-x64-debug\\examples\\Debug\\sonotide_render_silence.exe
cmd.exe /c "build\\msvc-x64-debug\\examples\\Debug\\sonotide_playback_session.exe C:\\Windows\\Media\\notify.wav"
```

## How to use the examples

- start with `list_devices` to confirm that the expected endpoints are visible;
- use `render_silence` to validate the basic WASAPI render path;
- use `playback_session` when you want to test the higher-level playback flow.
