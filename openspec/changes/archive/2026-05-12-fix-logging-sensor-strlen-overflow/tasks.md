## 1. Fix

- [x] 1.1 In `LoggingModuleSensor::__useExternal`, replace
  `char* c = new char[1]; c[0] = (char)index; string s(c); ... delete[] c;` with
  `string s(1, static_cast<char>(index));`.
- [x] 1.2 Apply the same change to `LoggingModuleSensor::__useInternal`.

## 2. Verification

- [x] 2.1 Rebuild `build-asan/`.
- [x] 2.2 Re-run `braitenberg_logging.xml` (500 iter, `--nogui`, ASan on) and confirm
  the heap-buffer-overflow at `LoggingModuleSensor.cpp:58` is gone.
- [x] 2.3 Same check for `braitenberg_noise.xml` and `hexapod_ralf.xml`.
- [x] 2.4 Confirm the 10 000-iter behavior regression remains byte-identical to
  `reference_logfile.csv` for the overlapping rows (fix must not change the token sent
  to `addMethod`, only how it is constructed).

## 3. Acceptance

- [x] 3.1 No `heap-buffer-overflow` reports remain that name `LoggingModuleSensor`.
- [x] 3.2 GUI smoke on `braitenberg_logging.xml` still produces a populated window and
  a clean `Good bye.` shutdown.
