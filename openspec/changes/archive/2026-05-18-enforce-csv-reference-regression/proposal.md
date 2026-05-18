# Enforce CSV reference regression check

## Why

`openspec/specs/xml-xsd-validation/spec.md` says:

> A deterministic simulation (`braitenberg_logging.xml`) SHALL produce
> CSV output byte-identical to the checked-in `xml/reference_logfile.csv`,
> or any difference SHALL be documented as intentional.

In practice the Linux CI workflow has the check, but it's
warning-level only. Two reasons it's never been promoted to fatal:

1. **Length mismatch.** `xml/reference_logfile.csv` is 10 002 rows
   captured at 10 000 iterations. CI bounds the run at 2 000
   iterations (the standalone-config audit budget). diff always
   fails on length.
2. **Cross-platform float noise.** The checked-in reference was
   produced on macOS arm64. Linux x86_64 emits minor signed-zero
   differences on what should be exactly zero (e.g., `-0.001` vs
   `-0.000` in the wheel-hinge applied-force column). Two different
   IEEE-754 rounding paths through Bullet + the controller plugin.

Result: the check is decorative. A real physics regression that
shifts trajectories by 1% would also pass with a `::warning::`.

We want to make the check actually catch regressions: bit-identical
within the same platform, with the reference set sized for the CI
budget.

## What changes

1. **Per-platform reference set.** Either commit two files
   (`xml/reference_logfile.linux-x86_64.csv`,
   `xml/reference_logfile.macos-arm64.csv`) or generate the
   reference on first run and verify a hash on subsequent runs.
2. **Match the iteration budget.** Either trim the reference to
   2 000 rows, or extend the CI run to 10 000 (the macOS workflow
   already runs longer; the Linux one is tighter on time).
3. **Promote the check from warning to error.** Workflow step
   exits non-zero on diff for the matching platform reference.
4. **Document determinism guarantees.** What in the simulator is
   deterministic (same seed → same output on same platform), what
   is not (cross-platform float bit-exactness), and how to
   regenerate references when the change is intentional.

## Non-goals

- Cross-platform bit-identical output. That's a different (much
  larger) effort and would require either compensated-summation
  math or a software FPU.
- Pinning Bullet to a specific version that produces the current
  numbers. We already get the right ones from the submodule pin;
  the issue is purely cross-architecture.

## Open questions

- Is 2 000 iterations enough to catch most regressions? Probably
  yes for the braitenberg test (sensors stabilise within a few
  hundred steps), but a deliberate trajectory drift after that
  point would be missed.
- Should the reference live as plain CSV or as a hash file
  (sha256 of the canonical output)? Plain CSV makes diffs
  readable; hash makes diffs invisible until intentional.
