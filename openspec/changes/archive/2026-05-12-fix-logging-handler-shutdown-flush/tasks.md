## 1. Fix

- [x] 1.1 Add `YarsLoggingModel::flush()` that calls `LoggingHandler::close()` once
  (idempotent via a `_closed` flag).
- [x] 1.2 In `YarsLoggingControl::quit()`, invoke `_model->flush()` if `_model` is
  non-null.

## 2. Verification

- [x] 2.1 Rebuild `build/` (non-sanitizer).
- [x] 2.2 Re-run `braitenberg_logging.xml` for 10 000 iter with the controller lib and
  confirm the CSV is **byte-identical** to `reference_logfile.csv` end-to-end
  (`cmp reference_logfile.csv braitenberg-*.csv`).
- [x] 2.3 Rebuild `build-asan/`; re-run the same and confirm the ASan output is also
  byte-identical to the reference.

## 3. Acceptance

- [x] 3.1 The 10 000-iter regression CSV matches the reference byte-for-byte (no
  truncation, no extra rows).
- [x] 3.2 GUI smoke on `braitenberg_logging.xml` exits cleanly with no double-close
  warnings.
