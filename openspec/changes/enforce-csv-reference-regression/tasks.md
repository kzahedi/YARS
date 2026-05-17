# Tasks

## 1. Decide policy

- [ ] 1.1 Per-platform reference vs. cross-platform hash with
      acceptance tolerance. Recommend: per-platform CSV (readable
      diffs, no tolerance fudge factor).
- [ ] 1.2 Choose iteration count. Recommend: 2 000 to match the
      Linux audit budget; macOS workflow runs the same so the
      reference sizes match.
- [ ] 1.3 Document the determinism contract in
      `openspec/specs/xml-xsd-validation/spec.md`: same seed +
      same platform + same compiler standard library → same
      output. Cross-platform differences are documented and
      tolerated.

## 2. Generate references

- [ ] 2.1 On macOS arm64, run the simulator for the chosen
      iteration count and commit
      `xml/reference_logfile.macos-arm64.csv`.
- [ ] 2.2 On Linux x86_64 (CI runner), generate the equivalent
      file and commit `xml/reference_logfile.linux-x86_64.csv`.
      Easiest path: run the workflow with a side step that
      uploads the produced CSV as an artifact, download it, commit
      it.
- [ ] 2.3 Retire `xml/reference_logfile.csv` (the unsuffixed one),
      or keep it as an alias for the macOS reference for tools
      that still expect the old name.

## 3. Workflow changes

- [ ] 3.1 `.github/workflows/linux-build.yml`: change the diff to
      use `xml/reference_logfile.linux-x86_64.csv` and exit
      non-zero on mismatch (drop the `::warning::` fallback).
- [ ] 3.2 Add the equivalent step to
      `.github/workflows/macos-build.yml` against
      `xml/reference_logfile.macos-arm64.csv`.
- [ ] 3.3 Document the regenerate-the-reference recipe in
      `docs/planning/v0.8.7-open-points.md` (or a new
      `docs/regression-references.md`).

## 4. Validation

- [ ] 4.1 Push the per-platform references, see both CIs go
      green.
- [ ] 4.2 Deliberately introduce a 1-LSB physics change locally,
      confirm CI fails on the diff.
- [ ] 4.3 Cross off the CSV item in
      `docs/planning/v0.8.7-open-points.md`.
