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

- [ ] 3.1 Find the print site (`grep -rn "Can't open display" src/`)
- [ ] 3.2 Trace the caller and identify which XML element triggers display init even with `--nogui`
- [ ] 3.3 Patch the caller to no-op under `--nogui`
- [ ] 3.4 Verify `braitenberg_noise.xml` runs to 500 iterations cleanly
- [ ] 3.5 Re-add `braitenberg_noise.xml` to `linux-build.yml`'s `CFG2LIB` map

## 4. Documentation

- [ ] 4.1 Remove the `# Three configs are temporarily excluded` comment block from `linux-build.yml` once all three are back in the audit
- [ ] 4.2 Note the underlying bug class in `CLAUDE.md`'s "Known Working Features" → "Future" section if it matters for contributors
