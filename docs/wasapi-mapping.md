# WASAPI Mapping

## Runtime

- `runtime::enumerate_devices()` -> `IMMDeviceEnumerator::EnumAudioEndpoints`
- `runtime::default_device()` -> `IMMDeviceEnumerator::GetDefaultAudioEndpoint`

## Render Stream

- endpoint activation -> `IMMDevice::Activate(IAudioClient)`
- format discovery -> `IAudioClient::GetMixFormat`
- requested-format check -> `IAudioClient::IsFormatSupported`
- event mode init -> `IAudioClient::Initialize(... AUDCLNT_STREAMFLAGS_EVENTCALLBACK ...)`
- buffer service -> `IAudioRenderClient`

## Capture Stream

- event mode init -> `IAudioClient::Initialize(... EVENTCALLBACK ...)`
- packet service -> `IAudioCaptureClient`
- packet loop -> `GetNextPacketSize / GetBuffer / ReleaseBuffer`

## Loopback Stream

- render endpoint selection
- init flag includes `AUDCLNT_STREAMFLAGS_LOOPBACK`
- packet service still uses `IAudioCaptureClient`

## Error Translation

- `HRESULT` is preserved in `error.native_code`
- public callers see domain categories instead of raw COM codes

