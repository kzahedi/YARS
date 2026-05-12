## 1. Fix

- [x] 1.1 In `LoggingModuleSensor::__useExternal(int index)` and `__useInternal(int
  index)`, store the token as dimension 0 (`'\0'`) when `index < 0`, matching the
  decoder's `_sensor->externalValue((int)argument[0])` expectation.
- [x] 1.2 In `DataObjectVelocitySensor::externalValue(int index)` and
  `internalValue(int index)`, return `0.0` if `index < 0 || index >= _dimension` to
  prevent silent OOB.

## 2. Verification

- [x] 2.1 Rebuild `build-asan/`.
- [x] 2.2 Re-run `hexapod_ralf.xml` (500 iter, `--nogui`, ASan on) and confirm the
  heap-buffer-overflow at `DataObjectVelocitySensor.cpp:162` is gone.
- [x] 2.3 Re-run the behavior regression (`braitenberg_logging.xml`, 10 000 iter, with
  controller lib) and confirm the overlapping rows remain byte-identical to
  `reference_logfile.csv`.

## 3. Acceptance

- [x] 3.1 No `heap-buffer-overflow` reports naming `DataObjectVelocitySensor`.
- [x] 3.2 GUI smoke on `hexapod_ralf.xml` still produces a populated window with the
  articulated hexapod rendered.
