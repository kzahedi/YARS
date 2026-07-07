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
