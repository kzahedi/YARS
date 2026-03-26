## Why

YARS has several legacy robot experiment XMLs (ant, hexapod variants) from MPI research that have never been ported to the current rosiml format. Running these experiments confirms the simulator works with non-trivial locomotion robots and provides immediate visual validation of the physics and rendering.

## What Changes

- Copy and adapt XML files for ant (4-legged) and hexapod (6-legged) experiments from `/Volumes/Eregion/svn/mpi/experiments/` into `xml/`
- Update XML schema version declarations from 0.8.38/0.8.39 to 0.8.41 and fix any structural differences
- Add or adapt no-controller configurations so the robots can run without an external controller (passive/gravity drop, or simple sine-wave open-loop controller built into the XML)
- Validate each experiment launches, renders, and simulates without errors

## Capabilities

### New Capabilities

- `ant-experiment`: 4-legged ant robot simulation, adapted from `experiments/ant/xml/ant_basic.xml` (old RoSiML format → rosiml 0.8.41)
- `hexapod-experiment`: 6-legged hexapod simulation, adapted from `experiments/prototype.hexapod/version4/xml/hexapod_reduced.xml` (rosiml 0.8.38 → 0.8.41)

### Modified Capabilities

## Impact

- New XML files added to `xml/` directory (no code changes expected)
- May require minor XSD/parser tolerance fixes if old element names differ from current schema
- No breaking changes to existing simulations
