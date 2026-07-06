# CI Quality Gates — Design

**Date:** 2026-07-06
**Status:** Approved (brainstorm session 2026-07-06)
**Sequencing:** Merges **first** of the 2026-07-06 sub-projects — the
hexapod reference and perf trend make the Bullet switch (quick hardening
batch) and the raycast optimization verifiable.

## Goal

Lock in three protections the project currently gets only from one-off
local effort: sanitizer coverage, regression coverage of the subsystems
where real bugs were found, and visibility into performance drift.

## Component 1 — Sanitizer job (Linux, every push)

A new `sanitize` job in the existing Linux workflow:

- Separate build directory configured with
  `-fsanitize=address,undefined`, `RelWithDebInfo` (readable stacks).
- LSan enabled (`detect_leaks=1`) — this finally delivers the leak check
  the May 2026 memory-safety audit deferred to Linux
  (`docs/planning/memory-safety-audit-status.md`).
- `halt_on_error=1`; any sanitizer report fails the job.
- Runs the same 17-config headless corpus the May audit used. The
  config list is checked in as a script (e.g. `scripts/sanitize-corpus.sh`)
  so local and CI runs are identical.
- Headless build without Ogre/SDL to keep runtime modest; GUI code
  remains covered by the normal build jobs.
- An empty suppressions file ships from day one
  (`scripts/sanitizer-suppressions.txt`) so future third-party noise
  has a designated home instead of blocking merges.

## Component 2 — Hexapod CSV regression gate

Reuses the machinery from the archived openspec change
`enforce-csv-reference-regression` exactly:

- New `xml/hexapod_logging.xml` sized to the 2000-iteration CI budget.
- Per-platform, CI-generated references:
  `xml/reference_logfile_hexapod.linux-x86_64.csv` and
  `xml/reference_logfile_hexapod.macos-arm64.csv`.
- Bit-exact diff on both runners, fatal on mismatch.

Rationale: hexapod exercises muscle actuators, hinge chains, and the
velocity-sensor path — the subsystems where the May audit found actual
bugs. Scope decision: hexapod only for now (each added config costs two
reference files of maintenance); a sensor-rich config may be added when
the raycast work needs it.

## Component 3 — Performance trend (warn-only)

- After the regression step, each runner times a 100k-iteration headless
  braitenberg run and computes steps/s.
- The result (timestamp, commit SHA, platform, steps/s) is appended to a
  per-platform history file on a dedicated `ci-perf-history` branch via
  a bot commit. A branch survives across runs (artifacts do not) and is
  diffable.
- If the new number is >25% below the rolling median of the last 20
  entries for that platform, the job emits a prominent `::warning::`
  with both numbers. **It never fails the build** — GitHub-hosted
  runners are too noisy for a hard gate; humans judge the trend.
- The measuring script lives in `scripts/` and runs identically on the
  M4, which is the high-fidelity instrument: CI catches drift, the M4
  measures deltas.

## Validation

- Both workflows green on a no-op PR (sanitizer passes, hexapod
  references generated and matching, first perf entries committed).
- A deliberate canary (e.g. temporarily reintroducing a known ASan
  finding on a scratch branch) confirms the sanitizer job actually
  fails when it should.

## Out of scope

- macOS sanitizer job (no LSan on arm64 macOS; roughly doubles added CI
  time for marginal coverage).
- Hard performance gates.
- Nightly/cron scheduling — everything runs per push/PR.
