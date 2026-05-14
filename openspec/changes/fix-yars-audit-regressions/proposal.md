# Fix YARS audit regressions surfaced by Linux CI

## Why

The Linux audit corpus (`.github/workflows/linux-build.yml`) found three
pre-existing bugs when running each XML config for 500 iterations under
`--nogui`. All three are excluded from the CI audit list as of commit
following the macOS+Linux smoke-test work, with placeholder `# excluded`
comments referencing this proposal.

These are real YARS bugs, not infrastructure problems. They were never
caught before because the headless audit corpus was never green and CI
hadn't been running.

## What changes

### 1. `pthread_mutex_lock` assertion under `--nogui` on Linux

**Affected configs:**
- `xml/braitenberg_light_source.xml`
- `xml/braitenberg_zoo.xml`

**Symptom (glibc 2.35 / Ubuntu 22.04):**
```
yars: pthread_mutex_lock.c:94: ___pthread_mutex_lock:
  Assertion `mutex->__data.__owner == 0' failed.
```

A YARS thread is locking a mutex while glibc thinks no thread owns it,
or the mutex is being locked recursively without `PTHREAD_MUTEX_RECURSIVE`
attributes. Likely candidates:
- `DataController::_osdMutex` in `src/yars/configuration/data/DataController.cpp:21`
- `ConfigurationContainer::_librariesMutex` and friends in
  `src/yars/configuration/container/ConfigurationContainer.cpp` — many
  pthread mutexes are init'd with default attrs

**Action:**
- Reproduce locally on Linux with both configs
- Use `valgrind --tool=helgrind` or `tsan` to localise the offending mutex
- Fix attributes / locking discipline, not by suppressing the assert
- Re-add both configs to the CI audit list

### 2. `Can't open display` is fatal under `--nogui`

**Affected config:** `xml/braitenberg_noise.xml`

**Symptom:**
```
Can't open display
exit: 0   (under SSH, falls into headless fine)
exit: 255 (under CI, fails the audit step)
```

The XML has `<screen … show="true">`. With `--nogui`, YARS should
ignore display-related XML config and proceed in headless mode (it does
for some configs, but not this one). Either:
- The `--nogui` path doesn't suppress display init for all XML shapes
- Or one of `noise.xml`'s elements unconditionally calls display code

**Action:**
- Find the `Can't open display` print site in YARS source
- Wrap it / its caller in a `--nogui` check
- Re-add `braitenberg_noise.xml` to the CI audit list

## Acceptance

- All three configs run cleanly under `--nogui` on Linux for 500 iterations
- CI audit list restored to its full corpus
- Root-cause fixed (not workaround / suppression)
