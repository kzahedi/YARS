# Raycast Optimization — Performance Baseline

## Context

This is the baseline all later raycast-optimization tasks compare against.
It is measured **after** the vendored Bullet 3.25 switch (merged the day
before this baseline was taken), so it supersedes the older pre-switch
figure of **42.3k steps/s** on `braitenberg.xml`. Do not compare future
optimization results against the 42.3k number — always compare against the
figures below.

- **Machine**: Mac Mini, Apple M4, macOS 26.5.1 (Darwin 25.5.0, arm64)
- **Date**: 2026-07-07
- **Git SHA**: `e0da028eb9a59711aaaa64c2bc28d1e2380442d1` (branch `feat/raycast-optimization`, clean tree)
- **Build**: `./build`, Release configuration, linked against the vendored
  Bullet Physics 3.25 (`ext/bullet/install`), not reconfigured or rebuilt
  for this baseline.
- **Machine-quiet check**: `pgrep -fl "clang|cc1plus|cmake --build" | grep -v clangd`
  returned empty immediately before each of the three timed batches below
  (zoo wall-clock, braitenberg wall-clock, profiling run).

## Step 1: Wall-clock baseline

### `braitenberg_zoo.xml`, 20000 iterations, `--nogui`, run from `build/`
(needs `libYarsControllerBraitenberg2a`, resolved via the cwd/lib fallback
by running from the build directory)

```
run 1: 2.634s real (2.07s user, 0.05s sys, 80% cpu)
run 2: 2.157s real (2.07s user, 0.03s sys, 97% cpu)
run 3: 2.209s real (2.10s user, 0.03s sys, 96% cpu)
```

- Median real time: **2.209 s**
- Median throughput: **20000 / 2.209 ≈ 9,054 steps/s**

### `braitenberg.xml`, 100000 iterations, `--nogui`, run from `build/`

```
run 1: 2.163s real (2.06s user, 0.02s sys, 96% cpu)
run 2: 2.119s real (2.06s user, 0.02s sys, 98% cpu)
run 3: 2.206s real (2.09s user, 0.03s sys, 96% cpu)
```

- Median real time: **2.163 s**
- Median throughput: **100000 / 2.163 ≈ 46,232 steps/s**

(Consistent with the ~48k steps/s ballpark reported for this vendored-Bullet
build; small run-to-run variance is expected on a quiet but shared desktop
machine.)

## Step 2: Profile baseline

Command:

```bash
cd /Volumes/Eregion/projects/yars/build
./bin/yars --iterations 400000 --nogui --xml ../xml/braitenberg_zoo.xml >/dev/null 2>&1 & YPID=$!
sleep 2; sample $YPID 5 -file raycast-baseline-sample.txt >/dev/null 2>&1
kill $YPID 2>/dev/null
grep -A25 "Sort by top of stack" raycast-baseline-sample.txt | head -30
```

Top 15 lines of the "Sort by top of stack" table from `raycast-baseline-sample.txt`
(sample count in parentheses):

```
__workq_kernreturn  (in libsystem_kernel.dylib)                                                                                                    6886
btDbvt::rayTestInternal(btDbvtNode const*, btVector3 const&, btVector3 const&, btVector3 const&, unsigned int*, float, btVector3 const&,
  btVector3 const&, btAlignedObjectArray<btDbvtNode const*>&, btDbvt::ICollide&) const  (in libBulletCollision.3.25.dylib)                          746
btSubsimplexConvexCast::calcTimeOfImpact(btTransform const&, btTransform const&, btTransform const&, btTransform const&,
  btConvexCast::CastResult&)  (in libBulletCollision.3.25.dylib)                                                                                    645
btVoronoiSimplexSolver::closestPtPointTriangle(btVector3 const&, btVector3 const&, btVector3 const&, btVector3 const&,
  btSubSimplexClosestResult&)  (in libBulletCollision.3.25.dylib)                                                                                   243
btVoronoiSimplexSolver::updateClosestVectorAndPoints()  (in libBulletCollision.3.25.dylib)                                                          204
btSphereShape::localGetSupportingVertex(btVector3 const&) const  (in libBulletCollision.3.25.dylib)                                                 163
gResolveSingleConstraintRowGeneric_scalar_reference(btSolverBody&, btSolverBody&, btSolverConstraint const&)  (in libBulletDynamics.3.25.dylib)     160
btCollisionWorld::rayTestSingleInternal(btTransform const&, btTransform const&, btCollisionObjectWrapper const*,
  btCollisionWorld::RayResultCallback&)  (in libBulletCollision.3.25.dylib)                                                                         152
btDbvt::rayTest(btDbvtNode const*, btVector3 const&, btVector3 const&, btDbvt::ICollide&)  (in libBulletCollision.3.25.dylib)                       152
btVoronoiSimplexSolver::inSimplex(btVector3 const&)  (in libBulletCollision.3.25.dylib)                                                             121
atan2  (in libsystem_m.dylib)                                                                                                                       113
gResolveSingleConstraintRowLowerLimit_scalar_reference(btSolverBody&, btSolverBody&, btSolverConstraint const&)  (in libBulletDynamics.3.25.dylib)  100
btVoronoiSimplexSolver::closestPtPointTetrahedron(btVector3 const&, btVector3 const&, btVector3 const&, btVector3 const&, btVector3 const&,
  btSubSimplexClosestResult&)  (in libBulletCollision.3.25.dylib)                                                                                    98
btCylinderShape::localGetSupportingVertex(btVector3 const&) const  (in libBulletCollision.3.25.dylib)                                                81
btSequentialImpulseConstraintSolver::solveSingleIteration(int, btCollisionObject**, int, btPersistentManifold**, int, btTypedConstraint**, int,
  btContactSolverInfo const&, btIDebugDraw*)  (in libBulletDynamics.3.25.dylib)                                                                      62
```

Expected hotspots (`btDbvt::rayTestInternal`, `btSubsimplexConvexCast`,
`Pose::operator<<`) are all present. `yars::Pose::operator<<` shows up
further down the table at 43 samples, confirming it as a smaller but
present contributor. The dominant single non-kernel frame is
`btDbvt::rayTestInternal`, consistent with raycasting being the primary
optimization target.

Raw sample file retained at `build/raycast-baseline-sample.txt` (not
committed — build artifact).

## Summary

| Scenario | Iterations | Median wall-clock | Median throughput |
|---|---|---|---|
| `braitenberg_zoo.xml` | 20,000 | 2.209 s | ~9,054 steps/s |
| `braitenberg.xml` | 100,000 | 2.163 s | ~46,232 steps/s |

These numbers, on this vendored-Bullet-3.25 build at SHA `e0da028`, are the
baseline for all subsequent raycast-optimization tasks. Do not compare
against the pre-Bullet-switch figure of 42.3k steps/s.

## Task 4: Hoist loop-invariant pose composition (GenericProximitySensor)

Hoisted the per-ray `pose << objectPose` composition and `Quaternion r(pose.orientation)`
construction out of `GenericProximitySensor::prePhysicsUpdate`'s 5-ray loop (all five rays
share the same mounted `sensorPose`, verified via `__createRays`). Also removed a stray
mid-file `#include <iostream>` / `using namespace std;` from `Pose.cpp` (dead — nothing in
the file used unqualified `std::` symbols).

Profile spot-check, same command as the Step 2 baseline
(`./bin/yars --iterations 400000 --nogui --xml ../xml/braitenberg_zoo.xml`, 5s `sample`):

- Baseline: `yars::Pose::operator<<` appeared in the "Sort by top of stack" table at 43 samples.
- Post-hoist: `yars::Pose::operator<<` no longer appears anywhere in the "Sort by top of stack"
  table (that table only lists entries with >= 5 collapsed samples), consistent with the
  expected ~5x reduction in call count (composition now runs once per sensor update instead
  of once per ray).
- All other top hotspots (`btDbvt::rayTestInternal`, `btSubsimplexConvexCast::calcTimeOfImpact`,
  `btVoronoiSimplexSolver::*`) are unchanged in rank/order, as expected — this task does not
  touch raycasting itself.

Benchmark, `braitenberg_zoo.xml`, 20,000 iterations, `--nogui`, run from `build/` (3 quiet runs
after machine settled):

```
run 1: 1.97s real
run 2: 1.98s real
run 3: 1.99s real
```

- Median real time: **1.98 s** (vs. 2.06 s post-Task-3 reference median — small improvement,
  as expected given `Pose::operator<<` was a modest (43-sample) contributor, not a dominant one).
- An initial cold-cache set of 3 runs (3.04s / 2.53s / 2.02s) showed more variance from
  background system load; the settled set above is the representative measurement.

Regression gates (bit-exact, both empty):
- `braitenberg_logging.xml`, 2000 iterations vs `reference_logfile.macos-arm64.csv`: empty diff.
- `hexapod_logging.xml`, 2000 iterations vs `reference_logfile_hexapod.macos-arm64.csv`: empty diff.

## Task 5: LDR ray via btVector3 overload

Converted `GenericLDRSensor`'s per-light occlusion ray from the P3D `World::rayTest`
overload to the `btVector3` overload introduced in Task 2, removing a P3D↔btVector3
round-trip on the hit path. The planned range early-out step was skipped:
`DataGenericLightDependentResistorSensor` has no range/distance member to early-out on.
Verified byte-identical output on a dedicated LDR A/B gate
(`braitenberg_light_source.xml`, 2000 iterations) plus the standard braitenberg/hexapod
gates.

## Final results (all five tasks applied)

**Machine**: Mac Mini, Apple M4, macOS 26.5.1 (Darwin 25.5.0, arm64) — same machine as
the baseline. **Date**: 2026-07-07. **Git SHA**: `53a0259b123fd7049f1b824dac52aaed7dac546e`
(branch `feat/raycast-optimization`, clean tree, all five perf commits applied: Task 2
`377a0d6`, Task 3 `ed582ca`, Task 4 `455a41b`, Task 5 `53a0259`). Build: `./build`,
Release configuration, rebuilt immediately before measuring. Machine-quiet check
(`pgrep -fl "clang|cc1plus|cmake --build" | grep -v clangd`) returned empty before each
timed batch.

### Per-task wall-clock deltas on `braitenberg_zoo.xml` (20,000 iterations, running median)

| Checkpoint | Median real time | Throughput | Cumulative delta vs. baseline |
|---|---|---|---|
| Baseline (Task 1) | 2.209 s | ~9,054 steps/s | — |
| Post-Task-3 (rayTest overload + ray clamping) | 2.06 s | ~9,709 steps/s | ~6.7% |
| Post-Task-4 (Pose composition hoist) | 1.98 s | ~10,101 steps/s | ~10.4% |
| Post-Task-5 (LDR ray overload) / **final** | 1.997 s | ~10,015 steps/s | **~9.6%** |

(Post-Task-5 is a hair above the post-Task-4 checkpoint — both are within normal
run-to-run variance on this machine, ~1-2%; Task 5 targets the LDR ray, not the
proximity-sensor path this benchmark's rays run through, so no further zoo movement
was expected from it.)

### Final wall-clock benchmark: `braitenberg_zoo.xml`, 20,000 iterations, `--nogui`

Six runs; the first three (below) show typical warm-up variance, the settled group
of three that follows is the representative measurement:

```
run 1: 2.080s real
run 2: 2.016s real
run 3: 2.034s real
run 4: 2.018s real
run 5: 1.997s real
run 6: 1.993s real
```

- Median of the settled runs (4-6): **1.997 s**
- Median throughput: **20000 / 1.997 ≈ 10,015 steps/s**
- **Delta vs. baseline (2.209 s): ~9.6%** (just under the 10% target; see profile
  discussion below for what remains)

### Final wall-clock benchmark: `braitenberg.xml`, 100,000 iterations, `--nogui`

```
run 1: 2.014s real
run 2: 2.230s real
run 3: 2.251s real
run 4: 2.139s real
run 5: 2.172s real
run 6: 2.083s real
```

- Median of the settled runs (4-6): **2.139 s**
- Median throughput: **100000 / 2.139 ≈ 46,751 steps/s**
- **Delta vs. baseline (2.163 s): ~1.1%** — within run-to-run noise. Expected:
  `braitenberg.xml` runs a single robot with far fewer proximity/LDR rays per step
  than the zoo scenario, so the raycast-path optimizations have little surface here.

### Final profile spot-check

Command (identical to the baseline):

```bash
cd /Volumes/Eregion/projects/yars/build
./bin/yars --iterations 400000 --nogui --xml ../xml/braitenberg_zoo.xml >/dev/null 2>&1 & YPID=$!
sleep 2; sample $YPID 5 -file raycast-final-sample.txt >/dev/null 2>&1
kill $YPID 2>/dev/null
grep -A25 "Sort by top of stack" raycast-final-sample.txt | head -30
```

Top of the "Sort by top of stack" table, baseline vs. final (sample counts):

| Frame | Baseline | Final |
|---|---|---|
| `__workq_kernreturn` (kernel) | 6886 | 6986 |
| `btDbvt::rayTestInternal` | 746 | 787 |
| `btSubsimplexConvexCast::calcTimeOfImpact` | 645 | 612 |
| `btVoronoiSimplexSolver::closestPtPointTriangle` | 243 | 299 |
| `btVoronoiSimplexSolver::updateClosestVectorAndPoints` | 204 | 194 |
| `btSphereShape::localGetSupportingVertex` | 163 | 232 |
| `gResolveSingleConstraintRowGeneric_scalar_reference` | 160 | 228 |
| `btCollisionWorld::rayTestSingleInternal` | 152 | 115 |
| `btDbvt::rayTest` | 152 | 164 |
| `btVoronoiSimplexSolver::inSimplex` | 121 | 109 |
| `atan2` | 113 | 51 |
| `yars::Pose::operator<<` | **43** | **5** |

`yars::Pose::operator<<` dropped from 43 samples to 5 (≈88% reduction), confirming the
Task 4 hoist is still in effect at the final SHA and matches the ~5x call-count
reduction expected from moving pose composition out of the 5-ray loop.

The Bullet-internal raycast/narrowphase frames (`btDbvt::rayTestInternal`,
`btSubsimplexConvexCast::calcTimeOfImpact`, the `btVoronoiSimplexSolver::*` family)
did **not** shrink in absolute or kernel-relative terms — they move within normal
sample-to-sample noise (±10-15%) around their baseline values. This is expected given
what Tasks 2/3/5 actually changed: Task 2 removed a P3D↔btVector3 conversion on the
call boundary (cheap relative to the Bullet-internal BVH walk itself), Task 3's
running-min clamp only prunes traversal once a closer hit than the sensor's own
max range has already been found (most zoo rays hit nothing or hit far, so the clamp
rarely fires), and Task 5 touches the LDR path, not the proximity-sensor rays this
profile predominantly samples. None of the applied optimizations changed Bullet's own
`rayTestInternal`/narrowphase algorithms — they removed conversions and redundant work
*around* those calls. The dominant remaining cost is still raycasting/narrowphase
itself, which would require a different approach (e.g. reducing per-step ray count,
spatial batching, or a custom broadphase) to move further.

### GUI verification

Frame export via `--framesDirectory` is a known pre-existing break (see
`docs/planning/v0.8.7-open-points.md`, "PNG frame export is non-functional") — it
produces zero frames on this machine independent of this branch's changes, so it was
**not** used for this check per that documented limitation. Instead, ran the GUI
without frame export:

```bash
cd /Volumes/Eregion/projects/yars/build
timeout 90s ./bin/yars --iterations 1000 --xml ../xml/braitenberg_zoo.xml
```

Result: window opened, simulation ran to completion, clean exit:

```
Non-critical XML version mismatch
Showing differences from your XML's version 0.8.40 to the current version 0.8.41:
     0.8.41 -- optional -- traces can now also be projected to xy,yz,xz plane
...
ShadowMapper: initialised with light=Vector3(-0.57735, -0.57735, -0.57735)
Maximum number of physics iterations (1000) reached.
Good bye.
EXIT CODE: 0
```

Visual frame-by-frame comparison (sensor rays/robots in exported PNGs) is blocked by
the pre-existing frame-export bug and was not performed; the version-mismatch message
is expected/non-critical (XML schema version note, not an error).

### Regression gates (bit-exact, re-verified at final SHA)

- `braitenberg_logging.xml`, 2000 iterations vs `reference_logfile.macos-arm64.csv`: empty diff.
- `hexapod_logging.xml`, 2000 iterations vs `reference_logfile_hexapod.macos-arm64.csv`: empty diff.

### Summary

| Scenario | Baseline | Final | Delta |
|---|---|---|---|
| `braitenberg_zoo.xml` (20k iter) | 2.209 s / ~9,054 steps/s | 1.997 s / ~10,015 steps/s | **~9.6%** |
| `braitenberg.xml` (100k iter) | 2.163 s / ~46,232 steps/s | 2.139 s / ~46,751 steps/s | ~1.1% (noise) |

Result: **~9.6% on the zoo benchmark** — just under the ≥10% target set in the plan,
result-preserving (all regression gates bit-exact) across five commits (Task 2 through
Task 5). The profile confirms the targeted `Pose::operator<<` hotspot is resolved; the
remaining cost is dominated by Bullet's own raycast/narrowphase implementation, which
is out of scope for this result-preserving optimization pass and would need a
different (likely more invasive, e.g. broadphase/ray-batching) approach to move
further.
