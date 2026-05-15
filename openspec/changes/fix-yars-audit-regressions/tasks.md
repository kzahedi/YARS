# Tasks

## 1. Reproduce and triage the pthread_mutex assertion

- [ ] 1.1 In the Linux UTM VM, run `./bin/yars --iterations 100 --nogui --xml ../xml/braitenberg_light_source.xml` and confirm the assertion fires
- [ ] 1.2 Same for `braitenberg_zoo.xml`
- [ ] 1.3 Add a temporary `#include <pthread.h>` + `pthread_mutexattr_settype(..., PTHREAD_MUTEX_ERRORCHECK)` patch in `DataController.cpp` to convert the assert into an `errno` return; rerun to see the error code (`EDEADLK` vs `EINVAL` vs `EPERM` localises the bug class)
- [ ] 1.4 If reproducible: run under `valgrind --tool=helgrind` to dump the offending lock site
- [ ] 1.5 Identify which mutex (in which file) is being locked incorrectly

## 2. Fix the pthread_mutex bug

- [ ] 2.1 Apply the minimal correct fix (likely: change attr to `RECURSIVE` if a single thread re-locks legitimately, or fix the locking call site if double-lock is the actual bug)
- [ ] 2.2 Verify on both `braitenberg_light_source.xml` and `braitenberg_zoo.xml`
- [ ] 2.3 Spot-check that `braitenberg.xml` and `braitenberg_logging.xml` (which were green) still pass after the fix
- [ ] 2.4 Re-add both configs to `linux-build.yml`'s `CFG2LIB` map

## 3. Fix the `Can't open display` fatal-under-nogui case

- [x] 3.1 Print site found in two places: `DataLoggingGnuplot.cpp:63` and `DataLoggingSelforg.cpp:59`
- [x] 3.2 The `<gnuplot>` → `<target>` element and `<selforg>` (matrixviz/guilogger) elements both unconditionally checked `$DISPLAY` and `exit(-1)`'d on non-Apple if it was empty
- [x] 3.3 Gated both checks on `__YARS_GET_USE_VISUALISATION` (commit bb659c5 + c909bfc); `--nogui` clears the flag via `setUseVisualisation(false)`, so the headless path now skips display-bound setup entirely
- [x] 3.4 Verified locally on UTM Ubuntu VM: `braitenberg_noise.xml` runs 100 iterations under `--nogui`, exit=0, no `Can't open display` message
- [x] 3.5 Re-added `braitenberg_noise.xml` to `linux-build.yml`'s `CFG2LIB` map

## 4. Documentation

- [ ] 4.1 Remove the `# Three configs are temporarily excluded` comment block from `linux-build.yml` once all three are back in the audit
- [ ] 4.2 Note the underlying bug class in `CLAUDE.md`'s "Known Working Features" → "Future" section if it matters for contributors
