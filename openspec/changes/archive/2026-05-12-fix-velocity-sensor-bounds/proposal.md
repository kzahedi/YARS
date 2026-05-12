## Why

The memory-safety audit (`add-memory-safety-audit`, run 2026-05-12) recorded an
AddressSanitizer heap-buffer-overflow at `DataObjectVelocitySensor.cpp:162`,
reading 8 bytes before the heap allocation backing `_value`. The trigger config is
`hexapod_ralf.xml`, which has a single-dimension velocity sensor.

Tracing the call chain:

```
DataObjectVelocitySensor::externalValue(int index)   // reads _value[index]
  ← LoggingModule::getSensorExternalValues(string argument)
       _sensor->externalValue((int)argument[0])
  ← addMethod was called with `argument = string(1, (char)index)`
  ← LoggingModuleSensor::__useExternal(-1)            // negative sentinel
  ← LoggingModuleSensor ctor, dimension==1 branch
```

When a sensor has a single dimension, the constructor passes `-1` as a sentinel meaning
"log the only dimension (dim 0)" — but the sentinel is then packed into the per-callback
token byte, so the LoggingModule reads back `-1` and indexes the value array at `-1`.

There are two layers of defect here:

1. The constructor at `LoggingModuleSensor.cpp:34,35` passes a sentinel that has no
   matching decoder downstream — the right token to store is `0`, the actual dimension
   we want to read.
2. `DataObjectVelocitySensor::externalValue` / `internalValue` index into `_value`
   without a bounds check, so the bug surfaces as silent corruption / sanitizer-only
   crash rather than a controlled error.

## What Changes

- Fix the sentinel at the call site: in `LoggingModuleSensor::__useExternal(int)` and
  `__useInternal(int)`, when `index < 0`, build the per-callback token as
  `string(1, '\0')` (dimension 0) instead of `string(1, static_cast<char>(-1))`. This
  makes the round-trip safe for every sensor type with `dimension == 1`.
- Add a defensive bounds check in `DataObjectVelocitySensor::externalValue` and
  `internalValue` so future regressions surface as a controlled error rather than
  silent memory corruption.

## Capabilities

### New Capabilities

<!-- None -->

### Modified Capabilities

<!-- None — behavior is unchanged for all configs whose sensors have dimension == 1
(those configs were silently reading whatever happened to live one slot before
_value[0]; they now read _value[0] as intended). -->

## Impact

- `src/yars/logging/LoggingModuleSensor.cpp` — 4 lines changed (two branches).
- `src/yars/configuration/data/DataObjectVelocitySensor.cpp` — 2 short bounds checks.
- Validation: rerun `hexapod_ralf.xml` under ASan; the OOB at line 162 must be gone.
