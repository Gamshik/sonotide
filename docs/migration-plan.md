# Migration Plan

## Goal

Extract WASAPI-specific responsibilities from the current app backend without a big-bang rewrite.

## Step 1

- switch device enumeration to Sonotide
- stop exposing direct `IMMDevice` knowledge in app-facing services

## Step 2

- migrate render output to `sonotide::render_stream`
- keep current Media Foundation source reading and DSP in the app temporarily

## Step 3

- introduce adapter layer from decoded PCM blocks into Sonotide render callback
- remove direct `IAudioClient` usage from `windows_streaming_audio_backend.cpp`

## Step 4

- move capture and loopback consumers over to Sonotide
- centralize device-loss recovery around Sonotide error/status reporting

## Step 5

- delete the legacy WASAPI backend from the app
- leave app code responsible only for transport, decode, DSP, and UX state

