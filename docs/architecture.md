# Architecture

## Layering

Public layer:

- `runtime`
- stream facade types
- device/format/error/status models

Internal orchestration layer:

- `detail::runtime_backend`
- stream worker-thread engine
- lifecycle state machine

Low-level Windows layer:

- COM bootstrap
- endpoint discovery and selection
- `IAudioClient` activation
- format negotiation
- event-driven render/capture packet flow

## Ownership

- `runtime` owns a shared backend boundary
- each stream object owns one `stream_handle`
- each running stream owns one worker thread and one stop event
- worker thread owns its COM apartment-scoped WASAPI interfaces

## Threading Contract

- `open_*_stream()` is synchronous and lightweight
- `start()` synchronously waits for worker initialization result
- callbacks run on the worker thread
- `stop()` joins the worker thread before returning
- `close()` is terminal and idempotent

## What Stays Outside Sonotide

- decoding from network or file formats
- DSP chains and app-specific equalizers
- UI/device preference persistence
- SoundCloud-specific transport and state coordination

