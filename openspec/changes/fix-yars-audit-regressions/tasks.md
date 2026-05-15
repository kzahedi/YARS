# Tasks

## 1. Reproduce and triage the pthread_mutex assertion

- [x] 1.1 Reproduced 100% deterministically on Ubuntu 22.04 arm64 (UTM): `pthread_mutex_lock.c:94 Assertion 'mutex->__data.__owner == 0' failed`
- [x] 1.2 Same assertion on `braitenberg_zoo.xml`
- [x] 1.3 Skipped the ERRORCHECK route — gdb backtrace was sufficient and cheaper
- [x] 1.4 Helgrind didn't reproduce (slowdown masked the race), but gdb backtrace pinpointed `DataGenericLightDependentResistorSensor::getInternalDomain → YM_LOCK → pthread_mutex_lock(&_mutex)`
- [x] 1.5 Mutex: `_mutex` declared in 17 Data* class headers (Actuator, AngularMotor, Generic*, Hinge*, Slider*, Muscle*, Object*Sensor, etc.) but never initialised via `pthread_mutex_init` or `PTHREAD_MUTEX_INITIALIZER`. The garbage in the uninitialised memory occasionally looked enough like an unlocked mutex that `lll_mutex_lock` succeeded — but the `__owner` field was non-zero, triggering the post-lock assertion.

## 2. Fix the pthread_mutex bug

- [x] 2.1 Applied C++11 in-class default-member-init with `PTHREAD_MUTEX_INITIALIZER` to all 17 affected headers (commit e1c9e6e). One-line edit per file, no constructor surgery needed; equivalent to `pthread_mutex_init(&m, NULL)` at construction time.
- [x] 2.2 Verified both configs run 500 iterations cleanly: `braitenberg_light_source.xml` exit=0, `braitenberg_zoo.xml` exit=0
- [x] 2.3 Spot-checked the previously-green configs (`braitenberg`, `braitenberg_logging`, `braitenberg_trace_projection`, `braitenberg_noise`, `muscle`) — all still exit 0 at 500 iterations
- [x] 2.4 Re-added both configs to `linux-build.yml`'s `CFG2LIB` map

## 3. Fix the `Can't open display` fatal-under-nogui case

- [x] 3.1 Print site found in two places: `DataLoggingGnuplot.cpp:63` and `DataLoggingSelforg.cpp:59`
- [x] 3.2 The `<gnuplot>` → `<target>` element and `<selforg>` (matrixviz/guilogger) elements both unconditionally checked `$DISPLAY` and `exit(-1)`'d on non-Apple if it was empty
- [x] 3.3 Gated both checks on `__YARS_GET_USE_VISUALISATION` (commit bb659c5 + c909bfc); `--nogui` clears the flag via `setUseVisualisation(false)`, so the headless path now skips display-bound setup entirely
- [x] 3.4 Verified locally on UTM Ubuntu VM: `braitenberg_noise.xml` runs 100 iterations under `--nogui`, exit=0, no `Can't open display` message
- [x] 3.5 Re-added `braitenberg_noise.xml` to `linux-build.yml`'s `CFG2LIB` map

## 4. Documentation

- [x] 4.1 Removed the temporary-exclusion comment block from `linux-build.yml` — all three configs back in the audit corpus
- [ ] 4.2 Note the underlying bug class in `CLAUDE.md`'s "Known Working Features" → "Future" section if it matters for contributors. **Pending** — low priority; the fix is now in the codebase and the audit will catch regressions.

---

This change is **complete**. All three originally-excluded configs run cleanly under `--nogui` for 500 iterations. Two real YARS bugs fixed:
1. Headless DISPLAY no longer fatal under `--nogui` (DataLoggingGnuplot + DataLoggingSelforg)
2. Uninitialised `_mutex` member in 17 Data* classes (PTHREAD_MUTEX_INITIALIZER in-class init)
