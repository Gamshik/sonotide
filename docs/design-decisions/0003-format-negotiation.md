# ADR 0003: Format Negotiation

Sonotide negotiates shared-mode formats against `IAudioClient::IsFormatSupported`. When the caller fully specifies sample type, sample rate, and channel count, the library attempts that format first. If fallback is allowed, the closest supported format or endpoint mix format is accepted explicitly and surfaced through `stream_status::negotiated_format`.

