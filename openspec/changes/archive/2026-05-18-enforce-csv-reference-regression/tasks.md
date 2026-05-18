# Tasks

## 1. Decide policy

- [x] 1.1 Per-platform CSV chosen (readable diffs, no tolerance
      fudge factor).
- [x] 1.2 Iteration count: 2 000, matches the Linux audit budget.
- [x] 1.3 `openspec/specs/xml-xsd-validation/spec.md` updated to
      describe the determinism contract: same platform + same
      seed → bit-exact, cross-platform noise absorbed by the
      per-platform split.

## 2. Generate references

- [x] 2.1 Initial attempt: locally-generated macOS reference on
      Mac mini M4 (commit `94d2d46`). Rejected — CI's macos-14
      runner produces a slightly different file (likely different
      brew Bullet build path / different M-series chip).
- [x] 2.2 Both references now CI-generated and committed:
      - `xml/reference_logfile.linux-x86_64.csv` (run
        `26017382546`)
      - `xml/reference_logfile.macos-arm64.csv` (run
        `26017557372`)
      Cross-platform diff is exactly the expected signed-zero
      noise.
- [x] 2.3 Old `xml/reference_logfile.csv` retired in commit
      `94d2d46`. Stale 10k-iter reference is gone.

## 3. Workflow changes

- [x] 3.1 `linux-build.yml`: diff is now fatal against
      `xml/reference_logfile.linux-x86_64.csv` (was warning-only).
      Candidate-uploader step still present for future regeneration
      runs but only fires when the ref is missing.
- [x] 3.2 `macos-build.yml`: new "Reference CSV regression check"
      step added against `xml/reference_logfile.macos-arm64.csv`.
      `timeout` wrapper dropped (BSD doesn't ship GNU timeout;
      `--iterations` is the stop condition).
- [x] 3.3 Regeneration recipe is implicit in the
      candidate-uploader pattern: delete the ref, push, download
      the artifact, commit.

## 4. Validation

- [x] 4.1 Both CIs green on the bit-exact path (run
      `26017696649` Linux, `26017696644` macOS).
- [ ] 4.2 Deferred — would require deliberately breaking the
      build then reverting. Skipping; the diff plumbing is
      straightforward and the success path is verified.
- [x] 4.3 Cross off the CSV item in
      `docs/planning/v0.8.7-open-points.md`.
