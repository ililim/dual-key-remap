# Development

## Build & Test

Run tests:
```
cmd.exe /c "call \"C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat\" -arch=x64 -host_arch=x64 -no_logo && cd /d C:\Users\marcin\dev\dual-key-remap && cl tests.c && .\tests.exe"
```

Gotchas when running from MINGW bash:
- `cmd.exe /c run-tests.bat` fails because bash can't resolve the bat file path across shell boundaries. Must inline the full command.
- `tests.exe` alone fails in cmd.exe because `.` is not in PATH by default. Must use `.\tests.exe`.
- The `call` keyword is required before `VsDevCmd.bat` so cmd.exe returns to execute the remaining `&&` chain (without `call`, cmd.exe transfers control and never comes back).

## Code Style

Pragmatic C in the style of Linus + DHH. Minimal abstractions, clean data structures, no unnecessary complexity.

- No em dashes in comments
- Flat structs over deep nesting. Data should be obvious at a glance.
- No premature abstraction. Three similar lines > a helper used once.
- Early returns over deep if/else chains
- `0` over `NULL` for pointer zeroing
- `g_` prefix for globals
- `// Section Name` + `// ------` dividers to organize files
- `/* @return block_input */` style doc comments only where non-obvious
- Sentinel values (`-1`) for "not yet configured" vs `0` for "configured as empty/NOOP"
- Parse functions return count on success, 0 for empty/NOOP, -1 on error
- Static for file-local helpers, extern for cross-file interfaces
- Function pointers for host-provided platform code (get_time_ms, send_input)
- Config parsing in the same file as the logic it configures (remap.c owns both)
- Tests use a DSL: `IN(KEY,DIR)`, `SEE(KEY,DIR)`, `EMPTY()`, `SECTION("title")`
- Keep tests readable as specifications: config block, input sequence, expected output

## Architecture

- `remap.c` - core state machine, parsing, event handling (no platform deps)
- `dual-key-remap.c` - Windows host (hooks, tray icon, send_input, get_time_ms)
- `tests.c` - test suite (provides its own send_input, get_time_ms)
- `keys.c` - key definitions and lookup
- `input.h` - shared Direction enum and send_input declaration

`remap.c` is `#include`d directly into both `dual-key-remap.c` and `tests.c`. Platform-specific code (time, window APIs) lives in the host, exposed to remap.c via `extern` function pointers.

## Releasing a New Version

### Version number

Set in `dual-key-remap.c` line 1: `#define VERSION "0.11"`. The release script reads this.

### Steps

1. Update `CHANGELOG.md` with all user-facing changes
2. Merge feature branch into `main`: `git checkout main && git merge <branch> --no-edit`
3. Tag: `git tag v<version>`
4. Push: `git push origin main --tags`
5. Build release: `make release` (from a VS dev prompt, or `cmd.exe /c run-release.bat`)
   - Kills running instance, builds exe, runs `release.ps1`
   - Creates `releases/dual-key-remap-v<version>/` with exe, README.txt, CHANGELOG.txt, LICENSE.txt, config.txt
   - Zips the folder to `releases/dual-key-remap-v<version>.zip`
   - Opens the folder in Explorer
6. Create GitHub release with zip attached:
   ```
   gh release create v<version> --title "v<version>" --notes "<body>" releases/dual-key-remap-v<version>.zip
   ```

### GitHub release body format

```
## Instructions
Download [dual-key-remap.zip](https://github.com/ililim/dual-key-remap/releases/download/v<version>/dual-key-remap-v<version>.zip) to install. Installation instructions can be found in the [README](https://github.com/ililim/dual-key-remap#installation).

<changelog section copied from CHANGELOG.md>
```

### Conventions

- Tag names: `v0.10` (lightweight tags, not annotated)
- Release titles: `v0.10` (older releases used "Version 0.X")
- Branch naming: feature branches named `v<version>`, merged to `main` at release
- One zip asset per release: `dual-key-remap-v<version>.zip`
- `config.example.txt` is shipped as `config.txt` in the release
