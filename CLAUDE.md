# Development

## Build & Test

Run tests: `cmd.exe /c run-tests.bat`

This sets up the Visual Studio x64 dev environment and runs `cl tests.c && .\tests.exe`.

## Code Style

Pragmatic C in the style of Linus + DHH. Minimal abstractions, clean data structures, no unnecessary complexity. No em dashes in comments.

## Architecture

- `remap.c` - core state machine, parsing, event handling (no platform deps)
- `dual-key-remap.c` - Windows host (hooks, tray icon, send_input, get_time_ms)
- `tests.c` - test suite (provides its own send_input, get_time_ms)
- `keys.c` - key definitions and lookup
- `input.h` - shared Direction enum and send_input declaration

`remap.c` is `#include`d directly into both `dual-key-remap.c` and `tests.c`. Platform-specific code (time, window APIs) lives in the host, exposed to remap.c via `extern` function pointers.
