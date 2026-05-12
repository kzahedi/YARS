## 1. Fix

- [x] 1.1 Add `_srcOffsetInWorldCoordinates = false;` and
  `_dstOffsetInWorldCoordinates = false;` to the `DataMuscleActuator(DataNode*)`
  constructor body (alongside the existing `false` initializations).

## 2. Verification

- [x] 2.1 Rebuild `build-asan/`.
- [x] 2.2 Re-run `muscle.xml` (500 iter, `--nogui`, ASan + UBSan on) and confirm both
  `runtime error: load of value … bool` reports are gone.
- [x] 2.3 Run a GUI smoke on `muscle.xml` (if it has a renderable scene) and confirm
  exit 0.

## 3. Acceptance

- [x] 3.1 No UBSan `not a valid value for type 'bool'` reports attributable to
  `DataMuscleActuator`.
