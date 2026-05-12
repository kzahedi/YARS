# Memory-safety audit — status

This document captures the result of the sanitizer-based memory-safety audit defined in
`openspec/changes/add-memory-safety-audit/`. The audit was run on **2026-05-12** against the
state on branch `feat/linux-ci-verification`.

## Build

- Toolchain: AppleClang 21.0.0.21000101 (Xcode), `arm64-apple-darwin25`
- CMake option: `YARS_ENABLE_SANITIZERS=ON`
- Flags applied: `-fsanitize=address,undefined -fno-omit-frame-pointer` (macOS — Apple
  libsanitizer ships ASan + UBSan but **not** LeakSanitizer; LSan portion is deferred to
  the Linux CI job from `add-linux-port-verification`).
- Build directory: `build-asan/` (separate from the production `build/`).
- Configure + build completed cleanly (no errors; warnings unchanged from non-sanitizer build).

## Runtime

- Iterations: 500 per config (controller-free) / 100 for the smoke test.
- Mode: `--nogui`
- ASan options: `abort_on_error=0:halt_on_error=0:detect_leaks=0`
- UBSan options: `print_stacktrace=1:halt_on_error=0`
- Per-run timeout: 180s (none was hit).

## Corpus coverage

17 standalone configs executed end-to-end. Controller-dependent and IPC-dependent configs
deferred — see `asan-reports/skipped.md`.

| Config                                    | Exit  | YARS-attributable findings              |
| ----------------------------------------- | ----- | --------------------------------------- |
| `ant.xml`                                 | 0     | XSD-parser-1 only                       |
| `braitenberg.xml`                         | 0     | XSD-parser-1 only                       |
| `braitenberg_light_source.xml`            | 0     | XSD-parser-1 only                       |
| `braitenberg_logging.xml`                 | 0     | XSD-parser-1, LoggingModuleSensor-1     |
| `braitenberg_nocontroller.xml`            | 0     | XSD-parser-1 only                       |
| `braitenberg_noise.xml`                   | 0     | XSD-parser-1, LoggingModuleSensor-1     |
| `braitenberg_trace_projection.xml`        | 0     | XSD-parser-1 only                       |
| `braitenberg_zoo.xml`                     | 0     | XSD-parser-1 only                       |
| `falling_objects.xml`                     | 0     | XSD-parser-1 only                       |
| `hexaboard.xml`                           | 0     | XSD-parser-1 only                       |
| `hexapod_crbm.xml`                        | 255   | XSD-parser-1 only (controller missing)  |
| `hexapod_low.xml`                         | 255   | XSD-parser-1 only (controller missing)  |
| `hexapod.xml`                             | 255   | XSD-parser-1 only (controller missing)  |
| `hexapod_ralf.xml`                        | 1     | XSD-parser-1, LoggingModuleSensor-1, ObjectVelocitySensor-1 |
| `joints/generic_angular.xml`              | 0     | XSD-parser-1 only                       |
| `joints/generic_force.xml`                | 0     | XSD-parser-1 only                       |
| `muscle.xml`                              | 0     | XSD-parser-1, MuscleActuator-uninit-2   |

Full per-config logs: `docs/planning/asan-reports/<config>.log`.

## Findings (YARS-attributable)

### 1. `XSD-parser-1` — global-buffer-overflow in YarsXSDSaxParser

- **Severity:** medium — read past 19-byte literal `"file:///schema.xsd"` on every config.
- **Location:** `src/yars/configuration/xsd/parser/YarsXSDSaxParser.cpp:60`
- **Cause:** narrow `char*` literal cast to `void*` and handed to
  `parser->setProperty(fgXercesSchemaExternalNoNameSpaceSchemaLocation, …)` which expects a
  `const XMLCh*` (UTF-16). Xerces walks the buffer as 16-bit code units looking for a
  double-zero terminator and reads ~20 bytes past the end of the literal.
- **Frequency:** every config (17/17 runs).
- **Fix sketch:** transcode via `XMLString::transcode("file:///schema.xsd")`, pass the
  resulting `XMLCh*`, then `XMLString::release` it after `loadGrammar`.
- **Follow-up:** ship as separate proposal `fix-xercesxsd-buffer-overflow` (not yet filed).

### 2. `LoggingModuleSensor-1` — heap-buffer-overflow on string construction

- **Severity:** medium — read past 1-byte heap allocation; only when an external sensor is logged.
- **Location:** `src/yars/logging/LoggingModuleSensor.cpp:56-58` (and the mirrored block in
  `__useInternal`)
- **Cause:** `char* c = new char[1]; c[0] = (char)index; string s(c);` — the `string(const char*)`
  constructor calls `strlen` on a 1-byte buffer with no null terminator.
- **Fix sketch:** `string s(1, static_cast<char>(index));` or pass the size: `string s(c, 1);`.
- **Follow-up:** ship as separate proposal `fix-logging-sensor-strlen-overflow`.

### 3. `MuscleActuator-uninit-2` — uninitialized bool read (UBSan, 2 sites)

- **Severity:** low-to-medium — `_srcOffsetInWorldCoordinates` and
  `_dstOffsetInWorldCoordinates` are not initialized in the constructor; UBSan observed value
  190 at first read. The values are typically set by the XML parser before use, but UBSan
  flags the existence of a read path that hits the uninitialized state.
- **Location:** `src/yars/configuration/data/DataMuscleActuator.cpp:308,310`; declarations at
  `DataMuscleActuator.h:209,211`; constructor at `DataMuscleActuator.cpp:81–111`.
- **Fix sketch:** default both members to `false` in the constructor initializer list.
- **Follow-up:** ship as part of the muscle-actuator audit (no proposal yet).

### 4. `ObjectVelocitySensor-1` — heap-buffer-overflow on `_value[index]`

- **Severity:** medium — read 8 bytes before the heap allocation backing `_value`.
- **Location:** `src/yars/configuration/data/DataObjectVelocitySensor.cpp:162`; allocation at
  line 147.
- **Cause:** `externalValue(int index)` performs `_value[index]` without bounds-checking
  `index` against `_dimension`. The reproduction in `hexapod_ralf` reads `_value[-1]`.
- **Fix sketch:** assert `0 <= index < _dimension` and audit the call chain
  (`LoggingModule::getSensorExternalValues`) to find where the bad index originates.
- **Follow-up:** ship as separate proposal `fix-velocity-sensor-bounds`.

## Findings (third-party — suppression candidates)

None observed in the 17-config run. The `XSD-parser-1` finding *surfaces* in
`libxerces-c-3.3.dylib` but the call site is YARS, so the fix belongs in YARS, not in a
suppression file.

If suppressions become necessary later, place them in `suppressions/asan.supp` and
`suppressions/valgrind.supp` and reference them via `ASAN_OPTIONS=suppressions=…`.

## Acceptance status

| Criterion (from `tasks.md` §5)                                  | Status |
| -------------------------------------------------------------- | ------ |
| All in-scope configs produce no YARS-attributable ASan reports | **fail** (4 findings, listed above; all routed to follow-up proposals) |
| Valgrind subset produces no "definitely lost" leaks (YARS)     | **deferred** to Linux CI (no arm64 macOS valgrind) |
| Status doc checked in and linked from `Refactoring_Plan.md`    | **done** |

The audit *did its job*: the smart-pointer migration produced a sanitizer-clean baseline
for the bulk of the simulation, and the four pre-existing defects above are now visible,
classified, and ready to be fixed under named follow-up proposals.

## GUI smoke verification (added 2026-05-12)

The ASan corpus runs all used `--nogui` per the audit spec. To confirm the smart-pointer
migration also leaves the render path working, four configs were re-run with GUI on
against the non-sanitizer `build/`:

| Config                       | Iter | Exit | Visual outcome                                    |
| ---------------------------- | ---- | ---- | ------------------------------------------------- |
| `braitenberg_logging.xml`    | 500  | 0    | clean shutdown, "Good bye."                       |
| `braitenberg_logging.xml`    | 30   | 0    | 36 frames captured, single Braitenberg + texture  |
| `braitenberg_zoo.xml`        | 30   | 0    | 36 frames, 4 robots with sensor cones, OSD        |
| `falling_objects.xml`        | 60   | 0    | 66 frames, sky + sandy ground + two falling balls |
| `hexapod_ralf.xml`           | 30   | 0    | 35 frames, articulated 6-leg robot rendered       |

No sanitizer-style aborts, no Ogre/SDL error messages in stderr. Window event handling
not exercised interactively (no mouse/keyboard events sent), only the auto-running
render loop.

## Reproduction

```bash
# Build (from repo root):
rm -rf build-asan && mkdir build-asan && cd build-asan
cmake -DYARS_ENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug ..
make -j4

# Single-config repro:
export ASAN_OPTIONS="abort_on_error=0:halt_on_error=0:detect_leaks=0"
export UBSAN_OPTIONS="print_stacktrace=1:halt_on_error=0"
./bin/yars --nogui --iterations 500 --xml ../xml/braitenberg_logging.xml
```

## Follow-up proposals to file

1. `fix-xercesxsd-buffer-overflow` — transcode the schema-location string.
2. `fix-logging-sensor-strlen-overflow` — drop the `new char[1]` + `string(const char*)` trick.
3. `fix-muscle-actuator-uninit-bools` — initialize the `_*InWorldCoordinates` members.
4. `fix-velocity-sensor-bounds` — bounds-check `_value[index]` and trace the bad index source.
