# Bullet SIMD Benchmark — Gate Decision for Source-Built Bullet

**Date:** 2026-07-06
**Machine:** Apple M4 (arm64), macOS. AppleClang 21.0.0.21000101.
**Benchmark:** `./bin/yars --iterations 100000 --nogui --xml xml/braitenberg.xml`, headless, Release build.

## Question

Does building Bullet 3.25 from source with `-O3` (vs. the Homebrew bottle)
give YARS a meaningful headless-physics speedup, justifying the switch from
the Homebrew package dependency to a vendored/source Bullet build (Task 6)?

Per the plan's own framing, this benchmark's likely and valid outcome is
NO-GO, because bullet3's `btScalar.h` already auto-enables NEON on Apple
arm64 single-precision builds regardless of who builds it — so there is no
"turn on SIMD" lever to pull here, only a possible difference in
optimization flags.

## Setup

### Homebrew Bullet (baseline)
- `brew list --versions bullet` → **3.25**
- Formula build args (`brew cat bullet`): `std_cmake_args` → `CMAKE_BUILD_TYPE=Release`,
  plus `-DBT_USE_EGL=ON -DBUILD_UNIT_TESTS=OFF -DINSTALL_EXTRA_LIBS=ON -DBULLET2_MULTITHREADING=ON`.
  The single-precision libs linked by YARS (`/opt/homebrew/opt/bullet/lib/libBullet*.dylib`)
  come from the formula's `build_shared` variant (also `-DBUILD_PYBULLET=ON`).
  bullet3's `CMakeLists.txt` does not override `CMAKE_CXX_FLAGS_RELEASE` for
  non-MSVC toolchains, so the *effective* optimization level should already
  be CMake's default Release flags — same `-O3 -DNDEBUG` as our source build.
- Verified arch: `file` → Mach-O arm64 (native, no Rosetta).
- YARS baseline binary: existing `./build` in the worktree (Release,
  `CMAKE_CXX_FLAGS_RELEASE=-O3 -DNDEBUG`, confirmed identical to the
  candidate build's `CMakeCache.txt`).

### Source Bullet 3.25 (candidate)
- Cloned `bulletphysics/bullet3` tag `3.25` into scratch
  (`/private/tmp/.../scratchpad/bullet-bench/bullet3`).
- Configured with (per brief, `CMAKE_POLICY_VERSION_MINIMUM=3.5` added only
  to satisfy this CMake version's minimum-version floor — no effect on
  compiled code):
  ```
  cmake -S bullet3 -B build -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=.../install -DBUILD_SHARED_LIBS=ON \
    -DBUILD_BULLET2_DEMOS=OFF -DBUILD_UNIT_TESTS=OFF \
    -DBUILD_CPU_DEMOS=OFF -DBUILD_OPENGL3_DEMOS=OFF \
    -DBUILD_EXTRAS=OFF -DBUILD_PYBULLET=OFF \
    -DUSE_DOUBLE_PRECISION=OFF -DCMAKE_CXX_FLAGS="-O3"
  ```
- No `-DBT_USE_NEON` passed, per the brief: NEON is auto-enabled by
  `btScalar.h` on Apple arm64 single-precision builds regardless, so it is
  on in *both* the Homebrew bottle and this source build. The only
  intentional experimental variable is optimization level/build options
  (explicit `-O3`, no EGL/multithreading/pybullet extras) — not SIMD
  enablement, which is a constant across both sides.
- **Precision check** (mismatch would be a silent ABI break — YARS defines
  `BT_USE_DOUBLE_PRECISION` nowhere):
  ```
  nm -C install/lib/libBulletDynamics.dylib | grep setDamping
  0000000000033cc4 T btRigidBody::setDamping(float, float)
  ```
  Confirmed single precision, matching Homebrew and YARS's expectations.
- Verified arch: Mach-O arm64 (native).

### YARS against source Bullet
- `cmake -S . -B build-bulletbench -DCMAKE_BUILD_TYPE=Release -DBULLET_ROOT=... -DCMAKE_PREFIX_PATH=.../install`
  — `BULLET_ROOT` alone was ignored (CMP0144 dev warning: "find_package is
  ignoring the variable"); `CMAKE_PREFIX_PATH` is what CMake's `FindBullet`
  module actually honors here. Configure log confirmed:
  `-- Found Bullet: .../scratchpad/bullet-bench/install/lib/libBulletDynamics.dylib`.
- Build succeeded (`cmake --build build-bulletbench -j 8`), no errors.
- `CMakeCache.txt` diff against `./build`: `CMAKE_BUILD_TYPE=Release` and
  `CMAKE_CXX_FLAGS_RELEASE=-O3 -DNDEBUG` identical in both — the YARS-side
  compile flags are apples-to-apples; only the linked Bullet library
  differs.
- **Linkage verification:**
  ```
  otool -L build-bulletbench/bin/yars | grep -i bullet
  @rpath/libBulletDynamics.3.25.dylib
  @rpath/libBulletCollision.3.25.dylib
  @rpath/libBulletSoftBody.3.25.dylib
  ```
  `@rpath` alone doesn't prove which copy loads — the binary's `LC_RPATH`
  list also includes `/opt/homebrew/lib`, which itself symlinks to the
  Homebrew Cellar Bullet libs (`/opt/homebrew/lib/libBulletDynamics.3.25.dylib -> ../Cellar/bullet/3.25/lib/...`).
  Confirmed the **actual runtime-loaded** library with
  `DYLD_PRINT_LIBRARIES=1 ./bin/yars --version`:
  ```
  .../scratchpad/bullet-bench/install/lib/libBulletDynamics.3.25.dylib
  .../scratchpad/bullet-bench/install/lib/libBulletCollision.3.25.dylib
  .../scratchpad/bullet-bench/install/lib/libLinearMath.3.25.dylib
  .../scratchpad/bullet-bench/install/lib/libBulletSoftBody.3.25.dylib
  ```
  The scratch install directory's `LC_RPATH` entry precedes
  `/opt/homebrew/lib` in the load-command order, so dyld resolves there
  first — confirmed the candidate binary is genuinely running against the
  source-built Bullet, not silently falling back to Homebrew's.

## Machine-quiet protocol

Before every timed run, confirmed no active compiles:
```
pgrep -fl "clang|cc1plus|cmake --build" | grep -v -E "clangd|grep"
```
(`clangd --background-index` is a persistent Xcode/editor process, not a
compile job, and was excluded from the match — otherwise the check would
never pass on this machine.) All six timed runs below were preceded by an
empty result from this check. Runs were interleaved
(baseline 1, candidate 1, baseline 2, candidate 2, baseline 3, candidate 3)
so any residual load bias affects both sides equally.

## Raw timing data

Wall-clock (`real`, via `/usr/bin/time -p`), `--iterations 100000 --nogui`:

| Run | Baseline (Homebrew) | Candidate (source, -O3) |
|-----|---------------------|--------------------------|
| 1   | 2.77 s              | 2.30 s                   |
| 2   | 2.34 s              | 2.07 s                   |
| 3   | 2.33 s              | 2.10 s                   |

Baseline run 1 (2.77s) is a clear cold-start outlier (first invocation after
the machine-quiet wait, disk cache cold); runs 2–3 are tight (2.33–2.34s)
and the median is insensitive to it regardless.

**Medians:**
- Baseline: 2.34 s → 100000 / 2.34 = **42,735 steps/s**
- Candidate: 2.10 s → 100000 / 2.10 = **47,619 steps/s**

**Delta: +11.4%** (candidate faster).

## Analysis

The result is a clean, reproducible win for the source build, not noise:
within-group spread is ~0.03s (excluding the one cold-start outlier) while
the between-group gap is ~0.24s — roughly 8x the noise floor — and it
replicated identically across all three interleaved pairs.

Per the brief's framing, this is **not** a NEON/SIMD story: NEON vector
math is already active in the Homebrew bottle (bullet3 auto-enables it for
Apple arm64 single-precision builds; nothing was toggled here — we
explicitly did not pass `-DBT_USE_NEON`, since there's no such flag to
flip). The measured delta must be attributed to optimization/build-option
differences instead:
- Explicit `-DCMAKE_CXX_FLAGS="-O3"` in the source build vs. no additional
  flags in Homebrew's formula. Both should already resolve to
  `CMAKE_CXX_FLAGS_RELEASE=-O3 -DNDEBUG` at the CMake-default level (bullet3
  doesn't override this for non-MSVC), so this alone shouldn't explain the
  gap.
- More plausibly: Homebrew's shared-library build variant (the one YARS
  actually links) is built with `-DBT_USE_EGL=ON -DBULLET2_MULTITHREADING=ON
  -DBUILD_PYBULLET=ON`, none of which are set in the source build. The
  multithreading option in particular can introduce thread-pool /
  synchronization scaffolding compiled into the solver path even in a
  single-threaded caller like YARS. This is a plausible but **unconfirmed**
  explanation — no code-path-level profiling was done to isolate it, and
  ruling it in/out was not in scope for this gate.
- Solver row functions remain scalar on ARM in both builds either way (SSE
  paths only, as the brief anticipated) — this is not a contributing
  factor.

## Decision

**Gate: ≥5% faster on braitenberg 100k headless steps/s → GO.**

Measured: **+11.4%**, comfortably above the 5% threshold.

**Verdict: GO.** Task 6 (switching YARS to build against a vendored/source
Bullet) should proceed.

Caveat for whoever executes Task 6: the mechanism behind the win is likely
Homebrew's non-default build options (multithreading/EGL/pybullet
scaffolding compiled into the shared libs YARS links), not NEON or `-O3`
per se — worth a quick confirmation pass (e.g. rebuild source Bullet with
`-DBULLET2_MULTITHREADING=ON` to see if the gap closes) before committing
to a specific vendored-build configuration, so Task 6 doesn't accidentally
reintroduce the very options responsible for the regression.

## Scratch artifacts (not committed)

- `/private/tmp/claude-501/.../scratchpad/bullet-bench/bullet3` (clone)
- `/private/tmp/claude-501/.../scratchpad/bullet-bench/build` (Bullet build dir)
- `/private/tmp/claude-501/.../scratchpad/bullet-bench/install` (Bullet install prefix)
- `/Volumes/Eregion/projects/yars-hardening/build-bulletbench` (YARS build dir against source Bullet — worktree-local, not committed; can be removed after Task 6 lands or this benchmark is superseded)
