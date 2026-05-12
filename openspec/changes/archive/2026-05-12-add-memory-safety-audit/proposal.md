## Why

The Phase 2 smart-pointer migration in the modernization plan transitioned ownership across the Robot, SceneGraph, and Configuration layers. The plan listed three validation items that were never closed out: no memory leaks under valgrind/ASan, all simulations run correctly, and no double-free crashes. With ownership semantics now mixed between `unique_ptr`, `shared_ptr`, and legacy raw-pointer macros in the configuration layer, an explicit memory-safety audit is needed before further refactoring proceeds.

## What Changes

- **Build with sanitizers**: Add a CMake preset / option that compiles YARS with AddressSanitizer + LeakSanitizer + UndefinedBehaviorSanitizer
- **Run the simulation-audit corpus under ASan**: Each XML config from `simulation-audit` is re-run with the ASan build for 500 iterations
- **Valgrind run on macOS-compatible subset**: Run a representative subset (`braitenberg_logging.xml`, `falling_objects.xml`, `braitenberg_zoo.xml`) under valgrind in headless mode
- **Capture and triage findings**: Any sanitizer reports are classified as leak / use-after-free / uninitialized-read / double-free; each gets a follow-up ticket or is fixed inline if trivial
- **Document the baseline**: A status doc records the sanitizer-clean configurations and any known suppressions

## Capabilities

### New Capabilities

- `memory-safety-audit`: Sanitizer-based verification that the smart-pointer migration produced no leaks, use-after-free, or double-free under the standard simulation corpus

### Modified Capabilities

<!-- None -->

## Impact

- `cmake/` — new `YARS_ENABLE_SANITIZERS` option, `-fsanitize=address,undefined,leak` flags
- `CMakeLists.txt` — sanitizer option wiring
- `docs/planning/memory-safety-audit-status.md` — new status doc
- Build directory: a new `build-asan/` directory is expected; existing `build/` is untouched
- No source-code changes from the audit itself; any defects found ship as separate change proposals
