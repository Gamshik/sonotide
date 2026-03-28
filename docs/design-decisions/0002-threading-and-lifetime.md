# ADR 0002: Threading And Lifetime

Each active stream owns a dedicated worker thread. Callbacks execute on that worker thread. `start()` waits for initialization, `stop()` joins, `reset()` clears negotiated runtime state, and `close()` is terminal. This keeps ownership explicit and avoids hidden background lifetimes.

