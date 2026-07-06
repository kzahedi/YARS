# Quick Hardening Batch Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Land four small hardening/perf items: `_groundShape` → `unique_ptr`, Socket/NamedPipe scratch-buffer cleanup, closure of the `Pose::operator<<` investigation, and a measurement-gated switch to a source-built SIMD Bullet.

**Architecture:** Items 1–3 are self-contained code cleanups, one commit each, reference-neutral. Item 4 first benchmarks a source-built Bullet against Homebrew's; only if ≥5% faster does it adopt the `ext/` submodule pattern already used for Ogre.

**Tech Stack:** C++17, Bullet 3.25, CMake 3.16+, git submodules, GitHub Actions.

**Spec:** `docs/superpowers/specs/2026-07-06-quick-hardening-batch-design.md`

## Global Constraints

- Work on branch `fix/quick-hardening` (CI triggers on `fix/**`).
- ALWAYS build and run yars in `./build`; ALWAYS set a timeout on yars runs.
- Regression check after every task: `cd build && rm -f braitenberg-*.csv && timeout 120s ./bin/yars --iterations 1000 --nogui --xml ../xml/braitenberg_logging.xml && diff <(head -1000 ../xml/reference_logfile.macos-arm64.csv) <(head -1000 braitenberg-*.csv)` — must be empty for Tasks 1–3. (The reference was generated with 2000 iterations; comparing the first 1000 lines of both is the correct local shortcut only if the file has one row per iteration — verify with `wc -l`; otherwise run the full 2000 iterations exactly as CI does.)
- Never claim success unless YARS compiles and runs without error; every task ends with a GUI smoke run.
- Commit per task; do NOT push without user approval.
- macOS: no `grep -P`.
- Merge AFTER the CI quality gates plan (`2026-07-06-ci-quality-gates.md`) so the hexapod reference + perf scripts exist.

---

### Task 1: `_groundShape` → `std::unique_ptr`

**Files:**
- Modify: `src/yars/physics/bullet/Environment.h:29`
- Modify: `src/yars/physics/bullet/Environment.cpp:11,18,49,57`

**Interfaces:**
- Produces: no interface change — `_groundShape` is a private member; the raw pointer is only passed to Bullet construction info inside `Environment.cpp`.

- [ ] **Step 1: Edit the header**

In `Environment.h`, add `#include <memory>` to the includes and change line 29:

```cpp
// before
btCollisionShape *_groundShape;
// after
std::unique_ptr<btCollisionShape> _groundShape;
```

- [ ] **Step 2: Edit the implementation**

`Environment.cpp` constructor (line 11): delete the line `_groundShape = NULL;` (a default-constructed `unique_ptr` is already null).

Destructor (line 18): delete the line `if (_groundShape != NULL) delete _groundShape;` (the comment above it about unique_ptr cleanup of Object entries stays).

Allocation (line 49):

```cpp
// before
_groundShape = new btStaticPlaneShape(btVector3(0,0,1),0);
// after
_groundShape = std::make_unique<btStaticPlaneShape>(btVector3(0, 0, 1), 0);
```

Use site (line 57): change `_groundShape` to `_groundShape.get()` in the `btRigidBody::btRigidBodyConstructionInfo` argument list.

Check for other uses: `grep -n "_groundShape" src/yars/physics/bullet/Environment.cpp` — every remaining use must be `.get()` (pointer arg) or implicit bool (`if (_groundShape)`).

- [ ] **Step 3: Build**

```bash
cd /Volumes/Eregion/projects/yars/build && cmake --build . -j 4
```

Expected: clean build, zero new warnings.

- [ ] **Step 4: Regression check (see Global Constraints) — must be bit-identical.**

- [ ] **Step 5: Commit**

```bash
git add src/yars/physics/bullet/Environment.h src/yars/physics/bullet/Environment.cpp
git commit -m "refactor(physics): _groundShape as unique_ptr; Bullet bodies only borrow shapes"
```

---

### Task 2: Socket scratch buffers

**Files:**
- Modify: `src/yars/util/Socket.cpp:189-245` (`Socket::operator>>(Buffer&)`)

**Interfaces:**
- Produces: no interface or wire-protocol change; byte layout identical.

- [ ] **Step 1: Confirm current behavior (no leak — this is an allocation-cost cleanup)**

```bash
grep -n "delete" src/yars/util/Socket.cpp | sed -n 1,20p
```

Verified 2026-07-06: `buf`, `type`, `sizeBytes` ARE freed (`Socket.cpp:242-244`; send path `:180`), and there are no early returns (the error handler calls `exit`). This task removes per-call heap allocation in a hot I/O path — it does NOT fix a leak; do not claim one.

- [ ] **Step 2: Replace heap scratch with stack arrays**

In `Socket::operator>>(Buffer &b)`:

```cpp
// before
char *buf       = new char[__BUFFER_SIZE];
char *type      = new char[1];
char *sizeBytes = new char[4];
// after
std::vector<char> bufStorage(__BUFFER_SIZE);
char *buf = bufStorage.data();
char type[1];
char sizeBytes[4];
```

(`__BUFFER_SIZE` may be large — check `grep -n "__BUFFER_SIZE" src/yars/util/Socket.cpp src/yars/util/*.h`; if it is ≤ 16384, a plain `char buf[__BUFFER_SIZE];` stack array is fine and preferred. `type`/`sizeBytes` are 1 and 4 bytes — always stack.) Add `#include <vector>` if using the vector form. Remove the `delete[]` block for these three at `Socket.cpp:242-244`. The `recv`/`__check`/`__coneverToInt` calls take the same pointers — `type` decays to `char*`, no call-site changes needed.

- [ ] **Step 3: Do the same audit for the send path**

`grep -n "new char" src/yars/util/Socket.cpp` — the send path allocates at `Socket.cpp:170` (freed at `:180`); convert it with the same pattern.

- [ ] **Step 4: Build + regression check + protocol test**

Build and run the standard regression check. Then, optionally, exercise the socket path:

```bash
cd /Volumes/Eregion/projects/yars/build
timeout 30s ./bin/yars --iterations 300 --nogui --xml ../xml/braitenberg_controller_tcpip.xml; echo "exit=$?"
```

Expected UNATTENDED: `exit=124` — the TCPIP controller (`contrib/controller/TCPIP.cpp`, port 9500) blocks in accept() until an external client connects; timing out is normal, not a failure. The real TCP peer is the `braitenbergCom` client (`contrib/braitenbergCom/Braitenberg.cpp`); if running it unattended isn't practical, rely on the nocontroller regression plus code review — the byte layout is untouched.

- [ ] **Step 5: Commit**

```bash
git add src/yars/util/Socket.cpp
git commit -m "perf(util): stack scratch buffers in Socket I/O paths; drop per-call heap allocation"
```

---

### Task 3: NamedPipe scratch buffers

**Files:**
- Modify: `src/yars/util/NamedPipe.cpp:80,101-103` and the surrounding `operator>>`/send path

**Interfaces:**
- Produces: no interface or wire-protocol change.

- [ ] **Step 1: Apply the identical transformation as Task 2**

`NamedPipe::operator>>(Buffer &b)` (lines ~98–140) has the same three allocations (`buf`, `type`, `sizeBytes`; freed at `:151-153` — like Socket, this is allocation cost, not a leak) — same replacement as Task 2. Line 80 has `char *buf = new char[b.size() + 1];` in the send path (freed at `:90`) — replace with `std::vector<char> bufStorage(b.size() + 1); char *buf = bufStorage.data();` and remove the matching `delete[]`.

- [ ] **Step 2: Build + regression check**

Standard regression check. The named-pipe controller config is `xml/braitenberg_controller_named_pipe.xml` — same caveat as Task 2 Step 4 about external peers.

- [ ] **Step 3: Commit**

```bash
git add src/yars/util/NamedPipe.cpp
git commit -m "perf(util): stack scratch buffers in NamedPipe paths; drop per-call heap allocation"
```

---

### Task 4: Close the `Pose::operator<<` investigation + fix adjacent UB

**Files:**
- Modify: `docs/planning/v0.8.7-open-points.md` (add one entry under "Latent issues noticed but not fixed")
- Modify: `src/yars/physics/bullet/GenericProximitySensor.cpp:9-17` (scalar-delete UB fix)

**Interfaces:** none.

**Finding (established 2026-07-06 during planning):** `yars::Pose::operator<<(const Pose&)` at `src/yars/types/Pose.cpp:84` is NOT stream output — it is pose composition (quaternion multiply, position transform). Its profile presence comes from `GenericProximitySensor::prePhysicsUpdate` calling it once per ray (5×/sensor/step) at `src/yars/physics/bullet/GenericProximitySensor.cpp:25-26`. The optimization (hoist the composition — all five rays share the same `sensorPose`) is Task 4 of the raycast plan (`docs/superpowers/plans/2026-07-06-raycast-optimization.md`), NOT this batch.

- [ ] **Step 1: Fix the scalar-delete UB found next door**

`GenericProximitySensor.cpp:9-10` allocates `new Pose[5]` / `new P3D[5]` but the destructor at `:16-17` frees them with scalar `delete` — undefined behavior; must be `delete[]`. (If the members are actually the `std::vector<RayCoordinates>` shown in the header, verify first with `sed -n 5,20p src/yars/physics/bullet/GenericProximitySensor.cpp` and fix whichever array-delete mismatch is real.) Build + standard regression check (bit-identical expected).

```bash
git add src/yars/physics/bullet/GenericProximitySensor.cpp
git commit -m "fix(sensors): delete[] for array allocations in GenericProximitySensor"
```

- [ ] **Step 2: Add the open-points entry**

Append to the "Latent issues noticed but not fixed" section of `docs/planning/v0.8.7-open-points.md`:

```markdown
- `Pose::operator<<(const Pose&)` (`src/yars/types/Pose.cpp:84`) showed up
  in the 2026-07-06 CPU profile. It is pose *composition* (not stream
  output), called once per ray per step from
  `GenericProximitySensor::prePhysicsUpdate`. Handled by the raycast
  optimization plan (hoisting), not the hardening batch. While reading
  the file: `Pose.cpp` has a stray `#include <iostream>` +
  `using namespace std;` in the middle of the file (line ~81) — remove
  with the raycast work.
- Pre-existing leak: the ground body's `groundMotionState`
  (`src/yars/physics/bullet/Environment.cpp:52`) is never freed —
  `Object.cpp:25` has the `delete _motionState;` commented out. Audit
  whether every Object leaks its motion state before fixing (Bullet
  ownership rules), then fix or file.
```

- [ ] **Step 2: Commit**

```bash
git add docs/planning/v0.8.7-open-points.md
git commit -m "docs(planning): Pose::operator<< profile finding — composition hot path, routed to raycast plan"
```

---

### Task 5: Bullet SIMD benchmark (the gate)

**Files:**
- Create: `/private/tmp/bullet-bench/` (scratch — NOT committed)
- Create: `docs/planning/bullet-simd-benchmark.md` (findings, committed)

**Interfaces:**
- Produces: a go/no-go decision recorded in `docs/planning/bullet-simd-benchmark.md`. Task 6 executes ONLY on "go".

- [ ] **Step 1: Baseline with Homebrew Bullet**

```bash
cd /Volumes/Eregion/projects/yars
for i in 1 2 3; do ./scripts/perf-measure.sh build; done
```

(Script from the CI quality gates plan. If not merged yet: `cd build && time ./bin/yars --iterations 100000 --nogui --xml ../xml/braitenberg.xml` three times and compute steps/s = 100000/elapsed.) Record the median.

- [ ] **Step 2: Build Bullet 3.25 from source**

```bash
mkdir -p /private/tmp/bullet-bench && cd /private/tmp/bullet-bench
git clone --depth 1 --branch 3.25 https://github.com/bulletphysics/bullet3.git
cmake -S bullet3 -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=/private/tmp/bullet-bench/install \
  -DBUILD_SHARED_LIBS=ON \
  -DBUILD_BULLET2_DEMOS=OFF -DBUILD_UNIT_TESTS=OFF \
  -DBUILD_CPU_DEMOS=OFF -DBUILD_OPENGL3_DEMOS=OFF \
  -DBUILD_EXTRAS=OFF -DBUILD_PYBULLET=OFF \
  -DUSE_DOUBLE_PRECISION=OFF \
  -DCMAKE_CXX_FLAGS="-O3"
cmake --build build -j 8 && cmake --install build
```

Precision: Homebrew's Bullet is SINGLE precision and YARS defines `BT_USE_DOUBLE_PRECISION` nowhere — `-DUSE_DOUBLE_PRECISION=OFF` pins that explicitly (a mismatch is a silent ABI break). Verify after install: `nm -C install/lib/libBulletDynamics.dylib | grep "setDamping"` must show `(float, float)`.

NEON: do NOT pass `-DBT_USE_NEON` — bullet3's `btScalar.h` auto-enables NEON on Apple arm64 single-precision builds, so the Homebrew bottle already has NEON vector math. The only real experimental variable here is optimization level (`-O3` vs the bottle's, typically `-Os`) — attribute any delta accordingly in the write-up, and expect the solver row functions to remain scalar on ARM either way (they have SSE paths only). A NO-GO outcome is the likely result; that is a valid, useful finding.

- [ ] **Step 3: Build YARS against the source Bullet and benchmark**

```bash
cd /Volumes/Eregion/projects/yars
cmake -S . -B build-bulletbench -DCMAKE_BUILD_TYPE=Release \
  -DBULLET_ROOT=/private/tmp/bullet-bench/install
cmake --build build-bulletbench -j 8
cd build-bulletbench
for i in 1 2 3; do time ./bin/yars --iterations 100000 --nogui --xml ../xml/braitenberg.xml; done
```

If `BULLET_ROOT` is not honored by the FindBullet module in use, check `grep -rn "Bullet" cmake/IncludePackages.cmake` and use `-DCMAKE_PREFIX_PATH=/private/tmp/bullet-bench/install` instead. Verify linkage: `otool -L build-bulletbench/bin/yars | grep -i bullet` must show `/private/tmp/bullet-bench/install/...`.

- [ ] **Step 4: Decide and document**

Compute median steps/s for both. Write `docs/planning/bullet-simd-benchmark.md` with: date, machine, Homebrew version+flags, source build flags, the two medians, the percentage delta, and the decision per the spec gate — **≥5% faster on braitenberg 100k headless steps/s → GO (execute Task 6); otherwise NO-GO (Task 6 is skipped; also update `docs/planning/v0.8.7-open-points.md` closing the item)**.

- [ ] **Step 5: Commit the findings**

```bash
git add docs/planning/bullet-simd-benchmark.md
git commit -m "docs(planning): Bullet SIMD benchmark — gate decision for source-built switch"
```

---

### Task 6: Bullet submodule switch (ONLY if Task 5 says GO)

**Files:**
- Create: `ext/bullet-source` (git submodule → https://github.com/bulletphysics/bullet3.git @ tag 3.25)
- Modify: `cmake/IncludePackages.cmake:22-28`
- Modify: `.github/workflows/linux-build.yml`, `.github/workflows/macos-build.yml`
- Modify: `CLAUDE.md` (prerequisites), `docs/planning/v0.8.7-open-points.md`

**Interfaces:**
- Produces: `ext/bullet/install/` per-platform Bullet install consumed by `find_package(Bullet)`.

- [ ] **Step 1: Add the submodule**

```bash
cd /Volumes/Eregion/projects/yars
git submodule add https://github.com/bulletphysics/bullet3.git ext/bullet-source
cd ext/bullet-source && git checkout 3.25 && cd ../..
git add .gitmodules ext/bullet-source
```

- [ ] **Step 2: Local build into ext/bullet/install**

Same cmake invocation as Task 5 Step 2 but `-S ext/bullet-source -B ext/bullet/build -DCMAKE_INSTALL_PREFIX=$(pwd)/ext/bullet/install`. Add `ext/bullet/build` and `ext/bullet/install` to `.gitignore` (check how `ext/ogre` is ignored and mirror it: `grep -n "ext/ogre" .gitignore`).

- [ ] **Step 3: Point CMake at it**

In `cmake/IncludePackages.cmake`, before `find_package(Bullet REQUIRED)` (line 23), insert:

```cmake
# Prefer the source-built SIMD Bullet (ext/bullet/install) when present;
# fall back to the system package otherwise. Mirrors the Ogre pattern.
if(EXISTS "${CMAKE_SOURCE_DIR}/ext/bullet/install")
  set(BULLET_ROOT "${CMAKE_SOURCE_DIR}/ext/bullet/install")
  list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/ext/bullet/install")
endif()
```

Rebuild from scratch (`rm -rf build && cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j 8`), verify linkage with `otool -L build/bin/yars | grep -i bullet` (must show `ext/bullet/install`).

- [ ] **Step 4: CI — both workflows**

In each workflow, after the Ogre cache/build block, add a Bullet cache/build block using the identical pattern (cache key `bullet325-<os>-${{ hashFiles('.gitmodules') }}-v1`, path `${{ github.workspace }}/ext/bullet/install`, conditional source build with the Task 5 Step 2 flags — Linux uses default SSE flags, i.e. drop `-DBT_USE_NEON=1` there). Remove `libbullet-dev` from the apt install list and `bullet` from the brew install list. Validate YAML as in the CI plan.

- [ ] **Step 5: CSV reference consequences**

Run the full regression locally (2000 iterations, exact CI procedure). If bit-identical: done. If it differs:
1. Qualitative check: plot/inspect that x/y trajectories evolve comparably (same row count, values diverge only gradually), and run the GUI: `cd build && timeout 60s ./bin/yars --iterations 500 --xml ../xml/braitenberg.xml` — robots must behave normally.
2. Regenerate references via the CI candidate-artifact flow (push branch with user approval; download `*-reference-candidate` artifacts for braitenberg AND hexapod from both platforms; commit them with message `test(xml): regenerate references for source-built SIMD Bullet`).

- [ ] **Step 6: Docs + final commit**

Update `CLAUDE.md` prerequisites (Bullet now built from `ext/bullet-source`, not brew/apt) and mark the item done in `docs/planning/v0.8.7-open-points.md`.

```bash
git add cmake/IncludePackages.cmake .github/workflows/ .gitignore CLAUDE.md docs/planning/v0.8.7-open-points.md
git commit -m "feat(build): source-built SIMD Bullet via ext/ submodule (gate: +N% on braitenberg 100k)"
```

Replace `N` with the measured delta.

---

### Final verification (whole batch)

- [ ] Clean build in `./build`; braitenberg + hexapod regression checks pass; GUI smoke run renders correctly; both CI workflows green after (approved) push.
