# Sensor Raycast Optimization Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Reduce sensor raycast cost — the largest CPU hotspot — with strictly result-preserving changes: cross-ray length clamping, per-call overhead removal, and pose-composition hoisting.

**Architecture:** All changes live in `GenericProximitySensor`, `GenericLDRSensor`, and `World`. Every task is gated by the bit-exact CSV regression (braitenberg + hexapod) and measured on `braitenberg_zoo.xml` (multi-robot, sensor-heavy — used instead of a new synthetic scene; the profile already shows raycasts dominating in existing scenes, so a new scene would add maintenance without new signal. This is a deliberate deviation from the spec's "add a benchmark scene").

**Tech Stack:** C++17, Bullet 3.25 (`btCollisionWorld::rayTest`, `ClosestRayResultCallback`).

**Spec:** `docs/superpowers/specs/2026-07-06-raycast-optimization-design.md`

## Global Constraints

- Branch: `feat/raycast-optimization` (CI triggers on `feat/**`).
- Result-preserving ONLY: after every task, the CSV regression must be bit-identical. If a change produces even a 1-ulp visible difference in the CSVs, revert that change — no reference regeneration in this plan.
- ALWAYS build/run in `./build`; ALWAYS timeout yars invocations.
- Regression command (run after every task):
  `cd build && rm -f braitenberg-*.csv && timeout 120s ./bin/yars --iterations 2000 --nogui --xml ../xml/braitenberg_logging.xml && diff ../xml/reference_logfile.macos-arm64.csv braitenberg-*.csv && echo BITEXACT`
  Plus the hexapod equivalent if `xml/reference_logfile_hexapod.macos-arm64.csv` exists (it does NOT yet exist at planning time — it lands with the CI quality gates plan; skip without ceremony if absent).
- Spec's "reuse/reset a ClosestRayResultCallback" item: intentionally declined — stack construction is cheap, and the `maxFraction` parameter on the Task 2 overload covers the useful part. Do not implement callback pooling.
- Benchmark command (median of 3):
  `cd build && time ./bin/yars --iterations 20000 --nogui --xml ../xml/braitenberg_zoo.xml`
- Commit per task; no pushes without user approval.
- If the source-built Bullet (hardening batch) merged first, re-run the Task 1 baseline — never compare across Bullet builds.

---

### Task 1: Baseline measurements

**Files:**
- Create: `docs/planning/raycast-baseline.md`

**Interfaces:**
- Produces: recorded baseline numbers every later task compares against.

- [ ] **Step 1: Wall-clock baseline**

```bash
cd /Volumes/Eregion/projects/yars/build
for i in 1 2 3; do time ./bin/yars --iterations 20000 --nogui --xml ../xml/braitenberg_zoo.xml; done
for i in 1 2 3; do time ./bin/yars --iterations 100000 --nogui --xml ../xml/braitenberg.xml; done
```

Record medians (zoo needs `libYarsControllerBraitenberg2a` via the cwd/lib fallback — run from `build/`).

- [ ] **Step 2: Profile baseline**

```bash
cd /Volumes/Eregion/projects/yars/build
./bin/yars --iterations 400000 --nogui --xml ../xml/braitenberg_zoo.xml >/dev/null 2>&1 & YPID=$!
sleep 2; sample $YPID 5 -file raycast-baseline-sample.txt >/dev/null 2>&1
kill $YPID 2>/dev/null
grep -A25 "Sort by top of stack" raycast-baseline-sample.txt | head -30
```

Record the top-of-stack table (expect `btDbvt::rayTestInternal`, `btSubsimplexConvexCast`, `Pose::operator<<` entries).

- [ ] **Step 3: Write `docs/planning/raycast-baseline.md`** with machine, date, git SHA, both medians, and the top-15 profile lines. Commit:

```bash
git add docs/planning/raycast-baseline.md
git commit -m "docs(planning): raycast optimization baseline (wall-clock + profile)"
```

---

### Task 2: `World::rayTest` btVector3 overload

**Files:**
- Modify: `src/yars/physics/bullet/World.h:26`
- Modify: `src/yars/physics/bullet/World.cpp:134-145`

**Interfaces:**
- Produces: `static bool World::rayTest(const btVector3 &start, const btVector3 &end, btVector3 &hitOut, btScalar maxFraction = btScalar(1.0))` — returns true and sets `hitOut` on a registered hit; returns false (hitOut = end) otherwise. `maxFraction < 1` presets the callback's hit fraction so Bullet prunes BVH traversal beyond it. The P3D overload delegates. Tasks 3–5 call the new overload.

**Precision context (verified 2026-07-06):** Bullet here is SINGLE precision (`btScalar` = float; no `BT_USE_DOUBLE_PRECISION` anywhere in the build). Two consequences baked into the code below: (a) the P3D overload's miss case must return the ORIGINAL double-precision `end`, never a float round-trip; (b) hit detection must use `m_collisionObject != nullptr`, because `hasHit()` is defined as `m_closestHitFraction < 1.0` and a preset fraction < 1 makes it return true spuriously.

- [ ] **Step 1: Add the overload**

`World.h` — next to line 26 (`btBulletDynamicsCommon.h` is already included at line 9, providing `btVector3`/`btScalar`):

```cpp
static P3D rayTest(P3D start, P3D end);                        // existing
static bool rayTest(const btVector3 &start, const btVector3 &end,
                    btVector3 &hitOut,
                    btScalar maxFraction = btScalar(1.0));     // new
```

`World.cpp`:

```cpp
bool World::rayTest(const btVector3 &start, const btVector3 &end,
                    btVector3 &hitOut, btScalar maxFraction)
{
  btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);
  if (maxFraction < btScalar(1.0))
    rayCallback.m_closestHitFraction = maxFraction;   // Bullet seeds traversal pruning from this
  _me->_world->rayTest(start, end, rayCallback);
  if (rayCallback.m_collisionObject != nullptr)       // NOT hasHit(): see precision context above
  {
    hitOut = rayCallback.m_hitPointWorld;
    return true;
  }
  hitOut = end;
  return false;
}

P3D World::rayTest(P3D start, P3D end)
{
  btVector3 _start(start.x, start.y, start.z);
  btVector3 _end(end.x, end.y, end.z);
  btVector3 hit;
  if (rayTest(_start, _end, hit)) return P3D(hit[0], hit[1], hit[2]);
  return end;   // exact original double value, no float round-trip
}
```

With the default `maxFraction = 1.0`, `m_collisionObject != nullptr` is equivalent to the old `hasHit()`, so every existing caller is bit-identical.

- [ ] **Step 2: Build + regression (must print BITEXACT). Commit:**

```bash
git add src/yars/physics/bullet/World.h src/yars/physics/bullet/World.cpp
git commit -m "perf(physics): btVector3 rayTest overload; P3D version delegates"
```

---

### Task 3: Cross-ray length clamping in the proximity sensor

**Files:**
- Modify: `src/yars/physics/bullet/GenericProximitySensor.cpp:39-58` (`postPhysicsUpdate`)

**Interfaces:**
- Consumes: `World::rayTest(const btVector3&, const btVector3&, btVector3&)` (Task 2).

- [ ] **Step 1: Rewrite `postPhysicsUpdate` — prune via `m_closestHitFraction`, do NOT shrink the segment**

Why not segment shrinking: (a) the original code does NOT skip misses — `World::rayTest` returns `end` on a miss and `|end − start|` lands at `fullLength ± ulp`, sometimes BELOW `fullLength`, in which case the original UPDATES the min; (b) Bullet computes `m_hitPointWorld = lerp(from, to, fraction)` in single-precision float, so a shrunk `to` changes registered hit points at ulp scale, and those values feed the controller (external sensor value → motors → physics), compounding over 2000 iterations into visible CSV divergence. Presetting the callback's hit fraction prunes traversal identically (Bullet seeds `m_lambda_max` from it) while keeping `from`/`to`/`fraction` — and therefore every registered hit point — bit-identical:

```cpp
void GenericProximitySensor::postPhysicsUpdate()
{
  const double fullLength = _data->distance();
  double length = fullLength;
  for (int i = 0; i < 5; i++)
  {
    const P3D &start = _rayCoordinates[i].pose.position;
    const P3D &endP  = _rayCoordinates[i].end;
    btVector3 bStart(start.x, start.y, start.z);
    btVector3 bEnd(endP.x, endP.y, endP.z);
    btScalar maxFraction = btScalar(1.0);
    if (length < fullLength)
      maxFraction = btScalar(length / fullLength + 1e-9);
    btVector3 bHit;
    const bool hitSomething = World::rayTest(bStart, bEnd, bHit, maxFraction);
    // Hit: identical value the unpruned query would produce.
    // Miss or pruned-beyond-threshold: replicate the ORIGINAL miss
    // arithmetic exactly — distance computed from the double-precision
    // `end`, which may update the min by an ulp, as the old code did.
    P3D hit = hitSomething ? P3D(bHit[0], bHit[1], bHit[2]) : endP;
    P3D diff = hit - _rayCoordinates[i].pose.position;
    double distance = diff.length();
    if (distance < length) length = distance;
  }
  _data->setMeasuredDistance(length);
  if (length < _data->distance())
    _data->setInternalValue(0, length);
  else
    _data->setInternalValue(0, _data->distance());
}
```

Exactness notes for the implementer:
1. Compare this against the CURRENT function body first (`sed -n 39,58p src/yars/physics/bullet/GenericProximitySensor.cpp`); every arithmetic step outside the pruning must match the original exactly (same `P3D` operations, same comparison order).
2. A real hit REJECTED by the preset fraction is treated as a miss here; case analysis says that cannot change the resulting min outside a pathological ulp window (rejected hits are farther than the current min). The regression check remains the arbiter: if ANY CSV byte differs, first try dropping the `+ 1e-9` epsilon; if it still differs, REVERT this task (keep Tasks 2/4/5) and record the finding with the diff in `docs/planning/raycast-baseline.md`. Expected outcome with this construction: PASS.

- [ ] **Step 2: Build + regression (BITEXACT required on braitenberg AND hexapod refs) + benchmark**

Run the Global Constraints regression and benchmark commands. Record the zoo median vs baseline.

- [ ] **Step 3: Sanity-test with a proximity-sensor scene**

`xml/braitenberg_logging.xml` itself uses 6 proximity sensors (and the zoo has 24), so the bit-exact regression in Step 2 is a REAL test of this change, not a formality. Additionally eyeball the logged sensor columns of the regression CSV: values must vary over time and stay within [0, distance].

- [ ] **Step 4: Commit**

```bash
git add src/yars/physics/bullet/GenericProximitySensor.cpp
git commit -m "perf(sensors): clamp proximity rays to running min — prunes BVH traversal, result-preserving"
```

---

### Task 4: Hoist pose composition in `prePhysicsUpdate`

**Files:**
- Modify: `src/yars/physics/bullet/GenericProximitySensor.cpp:20-38` (`prePhysicsUpdate`)
- Modify: `src/yars/types/Pose.cpp:81-83` (remove stray mid-file `#include <iostream>` / `using namespace std;`)

**Interfaces:** none new.

- [ ] **Step 1: Verify the invariant**

Read `GenericProximitySensor::__createRays` (lines ~60–90) and the `RayCoordinates` class (`GenericProximitySensor.h:14`): confirm every `_rayCoordinates[i].sensorPose` is assigned from the SAME `sensorPose` value (they are — `__createRays` copies one local into all five). If any per-ray difference exists, stop and keep the loop as-is.

- [ ] **Step 2: Hoist**

Current per-ray body recomputes `pose = sensorPose; pose << objectPose;` and constructs `Quaternion r(pose.orientation)` five times. Replace with one composition:

```cpp
void GenericProximitySensor::prePhysicsUpdate()
{
  Pose objectPose = _targetObject->data()->pose();
  // All five rays share the same mounted sensor pose; compose once.
  Pose composed = _rayCoordinates[0].sensorPose;
  composed << objectPose;                       // Pose composition (Pose.cpp:84)
  const Quaternion r(composed.orientation);
  const P3D rayTemplate(0, 0, _data->distance());
  for (int i = 0; i < 5; i++)
  {
    _rayCoordinates[i].pose = composed;
    Quaternion q = _rayCoordinates[i].q;
    q *= r;
    P3D ray = rayTemplate;
    ray *= q;
    _rayCoordinates[i].end = _rayCoordinates[i].pose.position + ray;
  }
}
```

IMPORTANT: this must reproduce the ORIGINAL arithmetic exactly. Open the current `prePhysicsUpdate` (lines ~20–38) and diff the operation order per ray: original does `q *= r; ray *= q;` where `r` is built from the composed pose's orientation and `q` starts as `_rayCoordinates[i].q`. The hoisted version keeps the identical per-ray sequence; only the composition (`<<`) and `r` construction move out of the loop — those were loop-invariant, so results are bit-identical. If the original code differs from this description in any way, replicate its exact order and only hoist provably invariant subexpressions.

- [ ] **Step 3: Clean `Pose.cpp`** — move `#include <iostream>` to the top of the file or delete it if now unused; delete the mid-file `using namespace std;` (check nothing below depends on it: `sed -n 84,100p src/yars/types/Pose.cpp`).

- [ ] **Step 4: Build + regression (BITEXACT) + benchmark + profile spot-check**

Re-run the Task 1 profile command; `Pose::operator<<` samples should drop roughly 5×. Record in `docs/planning/raycast-baseline.md`.

- [ ] **Step 5: Commit**

```bash
git add src/yars/physics/bullet/GenericProximitySensor.cpp src/yars/types/Pose.cpp docs/planning/raycast-baseline.md
git commit -m "perf(sensors): hoist loop-invariant pose composition out of per-ray loop"
```

---

### Task 5: LDR sensor overhead pass

**Files:**
- Modify: `src/yars/physics/bullet/GenericLDRSensor.cpp:40-55`

**Interfaces:**
- Consumes: `World::rayTest(const btVector3&, ...)` (Task 2).

- [ ] **Step 1: Convert the per-light ray to the btVector3 overload**

At `GenericLDRSensor.cpp:47` the sensor calls `World::rayTest(o, (*l)->position())` per light source. Convert start/end to `btVector3` once per light and call the Task 2 overload; keep the downstream math on the SAME values it received before. The miss value is LOAD-BEARING here: an unoccluded light is precisely a Bullet miss, and the occlusion test compares `hit.dist(o)` against the light distance (`GenericLDRSensor.cpp:47-55`) — on a miss you must use the original double-precision light position (the P3D overload's exact semantics), never a float round-trip.

- [ ] **Step 1b: Build a real regression gate for the LDR path**

The standard regression config (`braitenberg_logging.xml`) contains NO LDR sensors — it alone proves nothing about this task. Create the gate yourself: copy `xml/braitenberg_light_source.xml` (2 LDR sensors, deterministic, no noise) to the scratchpad, add a `<logging>` block mirroring `braitenberg_logging.xml`'s structure (log the LDR sensors' `<external/>` + body x/y, `<csv name="ldrgate">` with matching targets), generate a 2000-iteration reference CSV with the PRE-change binary (`git stash` the change if needed), then diff the POST-change output against it — must be bit-identical. This A/B file is a test fixture; don't commit it.

- [ ] **Step 2: Range early-out — only if the data model has one**

`grep -n "distance\|range" src/yars/configuration/data/DataGenericLightDependentResistorSensor.h` (find the actual header: `ls src/yars/configuration/data/ | grep -i ldr`). If the sensor's data model exposes a maximum range: skip the ray when `(lightPos - sensorPos).length() > range` AND verify by reading the current code that an out-of-range light contributes exactly the same value as an occluded one (only then is skipping result-preserving). If there is no range attribute or the semantics differ, SKIP this step and note why in the commit message.

- [ ] **Step 3: Build + regression (standard gate for no-collateral-damage, Step 1b gate for the LDR path itself) + benchmark. Commit:**

```bash
git add src/yars/physics/bullet/GenericLDRSensor.cpp
git commit -m "perf(sensors): LDR ray via btVector3 overload; optional range early-out"
```

---

### Task 6: Results write-up and acceptance

**Files:**
- Modify: `docs/planning/raycast-baseline.md` (final numbers)
- Modify: `docs/planning/v0.8.7-open-points.md` (close the Pose finding; record outcome)

- [ ] **Step 1: Final measurements** — re-run both wall-clock benchmarks and the profile; compute deltas vs Task 1 baseline. Target is ≥10% on the zoo benchmark; if lower, that's acceptable — record what was achieved and what the profile says remains.

- [ ] **Step 2: GUI verification with frame export (project completion bar)**

```bash
cd /Volumes/Eregion/projects/yars/build
mkdir -p raycast-frames
timeout 90s ./bin/yars --iterations 1000 --framesDirectory raycast-frames --xml ../xml/braitenberg_zoo.xml
ls raycast-frames | head
```

Expected: window opens, all robots move, sensor visualizations render, clean exit, and exported PNG frames show the scene correctly (open the last frame and check sensor rays/robots visually).

- [ ] **Step 3: Update both docs, commit:**

```bash
git add docs/planning/raycast-baseline.md docs/planning/v0.8.7-open-points.md
git commit -m "docs(planning): raycast optimization results — N% on braitenberg_zoo, bit-exact CSVs preserved"
```

- [ ] **Step 4: Hand off for push approval** (both CI workflows must go green before the branch is merge-eligible). Include the before/after profile samples and benchmark table in the PR description, per the spec's acceptance.
