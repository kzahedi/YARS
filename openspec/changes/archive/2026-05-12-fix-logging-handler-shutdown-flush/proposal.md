## Why

The behavior-regression check in `add-xml-xsd-validation` (run 2026-05-12) showed that
the 10 000-iteration `braitenberg_logging.xml` run produces a CSV that is byte-identical
to `reference_logfile.csv` for the first 9 994 rows but is **truncated mid-row** at row
9 995 — the last ~8 rows of the simulation are missing. The truncation reproduces on
both ASan and non-ASan builds (`cmp` confirms the two output files are byte-identical),
so it is a YARS defect, not a sanitizer artifact.

Tracing the shutdown sequence:

1. `YarsMainControl::run()` exits the physics loop after the final iteration.
2. `_ylc->quit()` is called (`YarsLoggingControl::quit()`) — **no-op**, it only documents
   that the model is owned elsewhere.
3. `__closeApplication()` runs `exit(0)` for headless mode.
4. `exit(0)` does not call destructors of heap-allocated objects (only for static-storage
   duration), so `~YarsLoggingModel()` never runs, so `__cleanup()` never runs, so
   `LoggingHandler::close()` never runs, so the per-logger `_output.close()` never runs.

`std::ofstream` keeps a 4-KB userspace buffer above the file descriptor. When `exit(0)`
runs without a flush, the kernel-level fd is closed but the userspace buffer is lost —
this matches the ~8 rows observed (each row is ~107 bytes, so 8 rows ≈ 856 bytes; the
buffer-fill heuristic is consistent).

## What Changes

- Add a flush+close call in `YarsLoggingControl::quit()` that invokes
  `LoggingHandler::close()` on the model's handler before `__closeApplication()` runs.
- Guard the call with a "not yet closed" flag so a subsequent destructor-driven cleanup
  doesn't double-close.

## Capabilities

### New Capabilities

<!-- None -->

### Modified Capabilities

<!-- None — output is now complete; the previously-lost trailing rows now land on disk. -->

## Impact

- `src/yars/logging/YarsLoggingControl.cpp` — call into the model to flush.
- `src/yars/logging/YarsLoggingModel.{h,cpp}` — new `flush()` method that runs
  `LoggingHandler::close()` once.
- Validation: rerun the 10 000-iter braitenberg behavior regression and confirm
  byte-identical to `reference_logfile.csv` end-to-end (10 002 lines, not 9 994).
