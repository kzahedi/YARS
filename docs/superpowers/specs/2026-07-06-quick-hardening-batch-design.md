# Quick Hardening Batch — Design

**Date:** 2026-07-06
**Status:** Approved (brainstorm session 2026-07-06)
**Sequencing:** Merge after the CI quality gates sub-project
(`2026-07-06-ci-quality-gates-design.md`), because the Bullet switch
benefits from the hexapod reference and perf-trend infrastructure.

## Goal

Clear four small hardening/performance items in one branch. Each item is
independently verifiable and lands as its own commit so any one can be
reverted alone. The Bullet SIMD switch is the only structurally
significant change and is gated on measurement.

## Item 1 — Bullet SIMD switch (gated)

**Background:** YARS links Homebrew's Bullet 3.25 bottle. A CPU profile
(headless braitenberg, 2026-07-06) shows the constraint solver running
`gResolveSingleConstraintRowGeneric_scalar_reference` — Bullet's scalar
fallback, not a SIMD path.

**Step 1 — verification gate.** Inspect the Homebrew build (symbols,
build flags) and build Bullet v3.25 from source with NEON/SIMD enabled.
Benchmark both against headless braitenberg (100k iterations) and
hexapod on the M4.

**Gate:** if the SIMD build is not ≥5% faster in headless steps/s on
the braitenberg 100k-iteration benchmark, stop — keep
Homebrew, close the item with a documented finding in
`docs/planning/v0.8.7-open-points.md`. No submodule is added.

**Step 2 — switch (only if the gate passes).** Follow the established
Ogre pattern:

- Add `ext/bullet-source` git submodule pinned at the upstream v3.25 tag.
- Build per platform into `ext/bullet/install` with SIMD/NEON on
  (macOS arm64: NEON; Linux x86_64: SSE/AVX per Bullet defaults).
- Point `find_package(Bullet)` at the install via `BULLET_ROOT` in
  `cmake/IncludePackages.cmake`, with the same style of fallback the
  Ogre integration uses.
- Update both CI workflows: build the submodule with caching (same
  approach as the Ogre cache), on macOS and Linux.
- Update build documentation (README / CLAUDE.md prerequisites).

**Floating-point consequences.** A different Bullet build may change
results at rounding level. Decision: **regenerate the reference CSVs**.
Procedure: verify trajectories are qualitatively identical (headless CSV
inspection + GUI smoke run with frame export), then regenerate both
platform reference logfiles via CI, for braitenberg and hexapod. The
bit-exact gate itself stays.

## Item 2 — `_groundShape` → `unique_ptr`

`src/yars/physics/bullet/Environment.h:29` holds a raw
`btCollisionShape*`. Ownership audit (2026-07-06): `Environment`
allocates the `btStaticPlaneShape` (`Environment.cpp:49`) and deletes it
in its destructor (`Environment.cpp:18`); Bullet rigid bodies reference
collision shapes but never own them. Change:

- Member becomes `std::unique_ptr<btCollisionShape>`.
- Construction sites use `std::make_unique<btStaticPlaneShape>(...)`.
- Rigid-body construction info receives `.get()`.
- Manual `delete` and `NULL` checks removed (note `Environment.cpp:11`
  still uses `NULL`; clean to `nullptr` handling while there).

## Item 3 — Socket/NamedPipe scratch buffers

`src/yars/util/Socket.cpp:193` and `src/yars/util/NamedPipe.cpp:102`
heap-allocate 1- and 4-byte `recv` scratch buffers (`new char[1]`,
`new char[4]`) — the same anti-pattern the May ASan audit fixed in
`LoggingModuleSensor`. Change:

- Replace with stack arrays (`char type[1]; char sizeBytes[4];` etc.).
- Audit the surrounding functions in both files for leaks on
  early-return paths and fix them.
- **No wire-protocol change**: byte layout is untouched, external
  controllers are unaffected.

## Item 4 — `Pose::operator<<` hot-path investigation (time-boxed)

`yars::Pose::operator<<` appeared in the CPU profile of a headless run.
Suspect: logging or trace formatting executing per step even when output
is disabled.

- Locate the per-step call site; determine whether the work is guarded.
- If the fix is a cheap guard or lazy formatting: apply it in this batch
  and confirm with a before/after profile sample.
- If structural: document the finding as a named follow-up in
  `docs/planning/v0.8.7-open-points.md` and stop. This item must not
  grow the batch.

## Validation (whole batch)

- Clean build in `./build`.
- Headless braitenberg 1000-iteration run diffed against the platform
  reference CSV — bit-exact, unless Item 1 legitimately regenerated the
  references.
- GUI run with frame export confirming correct rendering (project
  completion bar: GUI must work, not just `--nogui`).
- Both CI workflows green.

## Ordering within the batch

Items 2–4 first (reference-neutral), Item 1 last (the only one that may
touch references).

## Out of scope

- Any Bullet version upgrade beyond 3.25 (pin matches Homebrew's
  current version to isolate the SIMD variable).
- Multithreaded Bullet (`btITaskScheduler`) — small scenes make task
  overhead a likely net loss; not pursued.
