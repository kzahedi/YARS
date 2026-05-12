## Why

The memory-safety audit (`add-memory-safety-audit`, run 2026-05-12) recorded a UBSan
"load of value 190, which is not a valid value for type 'bool'" report at
`DataMuscleActuator.cpp:308` and `:310`, against members
`_srcOffsetInWorldCoordinates` and `_dstOffsetInWorldCoordinates`. Both members are
declared in `DataMuscleActuator.h` (`bool _srcOffsetInWorldCoordinates;` line 209,
`bool _dstOffsetInWorldCoordinates;` line 211) but never assigned a value in the
constructor at `DataMuscleActuator.cpp:81-111`.

In practice the XML parser sets them before `applyOffset` is called, so the simulation
output is not corrupted. But the read-before-init path exists, is reachable, and the
default contract for `bool` requires the value to be `0` or `1`. UBSan is right.

## What Changes

- Initialize `_srcOffsetInWorldCoordinates = false;` and
  `_dstOffsetInWorldCoordinates = false;` in the `DataMuscleActuator` constructor.

## Capabilities

### New Capabilities

<!-- None -->

### Modified Capabilities

<!-- None — behavior is unchanged for all configs where the XML supplies the flag. The
fix only specifies the default when the flag is absent. -->

## Impact

- `src/yars/configuration/data/DataMuscleActuator.cpp` — 2 lines added in the
  constructor body.
- Validation: rerun `muscle.xml` under ASan/UBSan and confirm both UBSan reports are
  gone.
