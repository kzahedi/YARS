# CI Quality Gates Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Add three CI protections: a Linux ASan+UBSan+LSan job over the headless corpus, a bit-exact hexapod CSV regression gate, and a warn-only performance trend.

**Architecture:** All work lives in `.github/workflows/` plus a new `scripts/` directory whose scripts run identically locally and in CI. The hexapod gate reuses the existing braitenberg reference-CSV machinery verbatim. Perf history persists on a dedicated `ci-perf-history` branch via bot commits.

**Tech Stack:** GitHub Actions (ubuntu-22.04 + macos-14 runners), bash, CMake 3.16+, ASan/UBSan/LSan (gcc), existing YARS build.

**Spec:** `docs/superpowers/specs/2026-07-06-ci-quality-gates-design.md`

## Global Constraints

- Work on branch `feat/ci-quality-gates` (CI push-triggers on `feat/**` — verified this works even though the default branch is `main`).
- Known trigger gap to fix while in these files: both workflows have `push: branches: [master, ...]` and `pull_request: branches: [master]`, but the default branch is `main` — merges to `main` currently run NO CI, and PR-event checks never fire. Add `main` to both lists in both workflows as part of Task 2 (one-line each), and note it in the commit message.
- ALWAYS build and run yars in `./build` (project rule).
- ALWAYS set a timeout when invoking `./bin/yars` (project rule; CI uses `timeout 60s`/`120s`).
- macOS shell rules: no `grep -P`; use `grep -E`.
- Never claim success unless YARS compiles and runs without error.
- Commit per task on the feature branch; do NOT push or open a PR without user approval.
- Existing reference-CSV pattern to imitate: the "Reference CSV regression check (braitenberg_logging.xml)" step in `.github/workflows/linux-build.yml` (lines ~186–220) and its macOS twin in `macos-build.yml`.
- The sanitizer corpus is the CI-runnable config set already exercised by the Linux audit steps (3 standalone + 9 controller configs listed in `linux-build.yml`), NOT the audit doc's "17 configs" — several of those 17 need local-only assets. This deviation is deliberate; note it in the commit message of Task 3.

---

### Task 1: Hexapod logging config

**Files:**
- Create: `xml/hexapod_logging.xml`
- Reference for structure: `xml/hexapod_shadow_test.xml` (808 lines, self-contained, NO controller — joints passive, robot falls under gravity; deterministic), `xml/braitenberg_logging.xml:297-310` (logging block syntax)

**Interfaces:**
- Produces: `xml/hexapod_logging.xml`, runnable headless with `--iterations 2000`, writing `hexapod-*.csv` in cwd. Task 2's CI step and the quick-hardening-batch plan both depend on this exact filename.

Scope note vs the spec: the spec's rationale mentions muscle-actuator coverage, but this scene has passive hinge joints only (no `<muscle>`, no controller — that's what makes it deterministic and CI-friendly). Muscle coverage comes from `xml/muscle.xml` in the sanitizer corpus instead. Record this in the commit message; the spec's intent (cover the subsystems where the audit found bugs — hinge chains, body dynamics, logging paths) is still served.

- [ ] **Step 1: Create the config**

Copy the shadow test scene and add a logging block:

```bash
cd /Volumes/Eregion/projects/yars
cp xml/hexapod_shadow_test.xml xml/hexapod_logging.xml
```

Edit `xml/hexapod_logging.xml`:

1. In the `<simulation ...>` root element region, find the `<screens>` section and set `show="false"` on the screen (headless-friendly; the CI run passes `--nogui` anyway, this just documents intent).
2. Keep the passive no-controller setup (comment near line 803 — it makes the run deterministic with zero controller-library dependencies). Insert the `<logging>` block at DOCUMENT level: between `</robots>` (line ~806) and `</rosiml>` (line ~808) — NOT inside `<robots>`. This matches braitenberg: `grep -n "</robots>\|<logging>" xml/braitenberg_logging.xml` shows `</robots>` at 280 and `<logging>` at 297.

```xml
  <logging>
    <object target="main" precision="3"> <use value="x"/> <use value="y"/> <use value="z"/> </object>
    <object target="head" precision="3"> <use value="x"/> <use value="y"/> <use value="z"/> </object>
    <object target="tail" precision="3"> <use value="x"/> <use value="y"/> <use value="z"/> </object>
    <csv name="hexapod">
      <target name="main"/> <target name="head"/> <target name="tail"/>
    </csv>
  </logging>
```

3. Syntax notes: the CSV output is produced by `<csv name="...">` with `<target name="..."/>` children referencing the logging modules above (see `xml/braitenberg_logging.xml:347-354` for the working example — mirror its exact structure; there is NO `<file>`-based CSV and NO `directory` attribute). With the default `date="true"` the output lands as `hexapod-<datetime>.csv` in the cwd, matching the `hexapod-*.csv` glob used below. The object targets `main`, `head`, `tail` exist as `<ply name=...>` in the scene (lines 71/81/91) and logging targets use plain body names.

- [ ] **Step 2: Run it and verify a CSV appears (this is the test)**

```bash
cd /Volumes/Eregion/projects/yars/build
rm -f hexapod-*.csv
timeout 120s ./bin/yars --iterations 2000 --nogui --xml ../xml/hexapod_logging.xml
ls hexapod-*.csv
```

Expected: exit 0, exactly one `hexapod-*.csv`. If XML validation fails, the error names the offending element — fix the logging block against the XSD semantics used by braitenberg_logging.xml.

- [ ] **Step 3: Verify the log is non-trivial and deterministic**

```bash
cd /Volumes/Eregion/projects/yars/build
head -3 hexapod-*.csv
awk 'NR==2{first=$0} END{print "rows:", NR; if($0==first) print "WARNING: last row equals first row"}' hexapod-*.csv
CSV1=$(ls -t hexapod-*.csv | head -1); mv "$CSV1" run1.csv
rm -f hexapod-*.csv
timeout 120s ./bin/yars --iterations 2000 --nogui --xml ../xml/hexapod_logging.xml
diff run1.csv hexapod-*.csv && echo DETERMINISTIC
```

Expected: >100 rows, values change over time (robot falls/settles), and `DETERMINISTIC` printed. If the diff fails, the scene has nondeterminism (e.g. a `<noise>` element) — find and remove it; a nondeterministic config cannot back a bit-exact gate.

- [ ] **Step 4: Commit**

```bash
cd /Volumes/Eregion/projects/yars
git add xml/hexapod_logging.xml
git commit -m "test(xml): hexapod logging config for CSV regression gate"
```

---

### Task 2: Hexapod CSV regression steps in both workflows

**Files:**
- Modify: `.github/workflows/linux-build.yml` (insert after the braitenberg "Reference CSV regression check" step and its artifact-upload step)
- Modify: `.github/workflows/macos-build.yml` (same position in that file)

**Interfaces:**
- Consumes: `xml/hexapod_logging.xml` (Task 1).
- Produces: CI steps expecting references at `xml/reference_logfile_hexapod.linux-x86_64.csv` and `xml/reference_logfile_hexapod.macos-arm64.csv`; candidate artifacts named `linux-x86_64-hexapod-reference-candidate` / `macos-arm64-hexapod-reference-candidate` until the references are committed.

- [ ] **Step 1: Add the Linux step**

Insert into `linux-build.yml` after the existing braitenberg reference-candidate upload step, mirroring the braitenberg step exactly (same env, same cd-into-build pattern):

```yaml
      - name: Reference CSV regression check (hexapod_logging.xml)
        env:
          LD_LIBRARY_PATH: ${{ env.OGRE_INSTALL_PREFIX }}/lib
        run: |
          cd build
          rm -f hexapod-*.csv
          timeout 120s ./bin/yars --iterations 2000 --nogui --xml ../xml/hexapod_logging.xml
          ACTUAL=$(ls -t hexapod-*.csv | head -1)
          if [ -z "$ACTUAL" ]; then
            echo "No hexapod-*.csv produced"
            exit 1
          fi
          mv "$ACTUAL" hexapod_logging_actual.csv
          cd ..
          REF=xml/reference_logfile_hexapod.linux-x86_64.csv
          if [ ! -f "$REF" ]; then
            echo "::warning::No Linux hexapod reference committed yet. Uploading candidate artifact."
            cp build/hexapod_logging_actual.csv linux-x86_64-hexapod-reference-candidate.csv
          else
            if diff -u "$REF" build/hexapod_logging_actual.csv > hexapod-csv-diff.log; then
              echo "Hexapod CSV byte-identical to $REF."
            else
              echo "::error::Hexapod CSV differs from $REF — see hexapod-csv-diff.log artifact."
              head -50 hexapod-csv-diff.log
              exit 1
            fi
          fi
```

Also add `hexapod-csv-diff.log` to each workflow's existing log-artifact upload path list (Linux: the `linux-audit-artifacts` step at `linux-build.yml:275-285`; macOS: `macos-build-logs`), so the error message's promise of an artifact is actually kept.

```yaml

      - name: Upload Linux hexapod reference candidate (only when ref missing)
        if: always()
        uses: actions/upload-artifact@v7
        with:
          name: linux-x86_64-hexapod-reference-candidate
          path: linux-x86_64-hexapod-reference-candidate.csv
          if-no-files-found: ignore
          retention-days: 14
```

- [ ] **Step 2: Add the macOS twin**

Open `.github/workflows/macos-build.yml`, locate its braitenberg reference step (search `reference_logfile.macos-arm64.csv`, around lines 94-121), and insert the analogous step after it with these substitutions: `REF=xml/reference_logfile_hexapod.macos-arm64.csv`, artifact name `macos-arm64-hexapod-reference-candidate`. IMPORTANT macOS differences (copy the macOS braitenberg step verbatim as the base, not the Linux one): it has NO `env:` block and NO `timeout` wrapper — BSD macOS doesn't ship GNU `timeout` (the workflow documents this at lines 97-99); the `--iterations 2000` bound is the run limiter.

- [ ] **Step 3: Validate workflow syntax locally**

```bash
cd /Volumes/Eregion/projects/yars
python3 -c "import yaml,sys; yaml.safe_load(open('.github/workflows/linux-build.yml')); yaml.safe_load(open('.github/workflows/macos-build.yml')); print('YAML OK')"
```

Expected: `YAML OK`.

- [ ] **Step 4: Commit**

```bash
git add .github/workflows/linux-build.yml .github/workflows/macos-build.yml
git commit -m "ci: hexapod CSV regression gate (reference-candidate flow until refs land)"
```

---

### Task 3: Sanitizer corpus script

**Files:**
- Create: `scripts/sanitize-corpus.sh`
- Create: `scripts/sanitizer-suppressions.txt` (empty placeholder-with-purpose: one comment line)

**Interfaces:**
- Produces: `scripts/sanitize-corpus.sh <build-dir>` — exit 0 iff every corpus config runs clean under the sanitizers. Task 4's CI job calls exactly this.

- [ ] **Step 1: Write the script**

```bash
#!/usr/bin/env bash
# Runs the CI-runnable config corpus under a sanitized YARS build.
# Usage: scripts/sanitize-corpus.sh <sanitized-build-dir>
# Any sanitizer report (halt_on_error=1) makes the run exit non-zero.
set -u
BUILD_DIR="${1:?usage: sanitize-corpus.sh <build-dir>}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SUPP="$ROOT/scripts/sanitizer-suppressions.txt"

export ASAN_OPTIONS="halt_on_error=1:detect_leaks=1:abort_on_error=1"
export UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1"
export LSAN_OPTIONS="suppressions=$SUPP"

# Standalone configs (no controller library needed)
STANDALONE=(
  xml/braitenberg_nocontroller.xml
  xml/falling_objects.xml
  xml/test_capture.xml
  xml/hexapod_logging.xml
)
# Controller-based configs: config -> lib name (mirrors linux-build.yml)
CONFIGS=(
  "xml/braitenberg.xml:YarsControllerBraitenberg2b"
  "xml/braitenberg_noise.xml:YarsControllerBraitenberg2b"
  "xml/braitenberg_logging.xml:YarsControllerBraitenberg3b"
  "xml/braitenberg_light_source.xml:YarsControllerBraitenberg2b"
  "xml/braitenberg_trace_projection.xml:YarsControllerBraitenberg2b"
  "xml/braitenberg_zoo.xml:YarsControllerBraitenberg2a"
  "xml/muscle.xml:YarsControllerSquareWave"
  "xml/joints/generic_angular.xml:YarsControllerSine"
  "xml/joints/generic_force.xml:YarsControllerSine"
)

FAILED=0
cd "$BUILD_DIR"
for cfg in "${STANDALONE[@]}"; do
  name=$(basename "$cfg" .xml)
  # Sanitized builds are ~2-4x slower: 240s cap, 500 iterations
  if timeout 240s ./bin/yars --iterations 500 --nogui --xml "$ROOT/$cfg" > "san-${name}.log" 2>&1; then
    echo "PASS $cfg"
  else
    echo "FAIL $cfg (see $BUILD_DIR/san-${name}.log)"; FAILED=1
  fi
done
for entry in "${CONFIGS[@]}"; do
  cfg="${entry%%:*}"; lib="${entry##*:}"
  name=$(basename "$cfg" .xml)
  if ! ls lib/lib${lib}.* >/dev/null 2>&1; then
    echo "FAIL $cfg (lib${lib} missing)"; FAILED=1; continue
  fi
  if timeout 240s ./bin/yars --iterations 500 --nogui --xml "$ROOT/$cfg" > "san-${name}.log" 2>&1; then
    echo "PASS $cfg"
  else
    echo "FAIL $cfg (see $BUILD_DIR/san-${name}.log)"; FAILED=1
  fi
done
exit $FAILED
```

`scripts/sanitizer-suppressions.txt`:

```
# LSan suppressions for third-party noise. Intentionally empty — add
# entries ONLY for leaks provably outside YARS code, with a comment
# naming the library and an upstream reference.
```

```bash
chmod +x scripts/sanitize-corpus.sh
```

- [ ] **Step 2: Smoke-test the script against the normal build (behavioral test of the harness, not the sanitizers)**

```bash
cd /Volumes/Eregion/projects/yars
./scripts/sanitize-corpus.sh build; echo "exit=$?"
```

Expected: PASS lines for every config, `exit=0` (all 13 listed configs verified present in the tree, 2026-07-06). macOS has no LSan; `detect_leaks=1` is ignored by a non-ASan binary — this step only proves corpus enumeration and exit-code plumbing.

- [ ] **Step 3: Commit**

```bash
git add scripts/sanitize-corpus.sh scripts/sanitizer-suppressions.txt
git commit -m "ci: sanitizer corpus runner (CI-runnable subset of the May audit corpus)"
```

---

### Task 4: Sanitizer job in the Linux workflow

**Files:**
- Modify: `.github/workflows/linux-build.yml` (new job `sanitize` parallel to `build-and-audit`)

**Interfaces:**
- Consumes: `scripts/sanitize-corpus.sh` (Task 3), `xml/hexapod_logging.xml` (Task 1).

- [ ] **Step 1: Add the job**

Append to `linux-build.yml` at the `jobs:` level (sibling of `build-and-audit`). The sanitized build is headless-only: `-DYARS_USE_VISUALISATION=OFF` (verified: `cmake/SetOptions.cmake:44`, default ON) skips SDL2/Ogre discovery and the view sources entirely. Note: no CI currently exercises the OFF path — if it has bitrotted, fix the trivial breakage if it's a missing include/guard; otherwise fall back to building WITH visualisation, copying BOTH the Ogre cache/build steps AND the full apt list (the GL/X11 packages: `libgl1-mesa-dev libglu1-mesa-dev libx11-dev libxt-dev libxrandr-dev libxinerama-dev libxi-dev libxcursor-dev`) from `build-and-audit`.

Use the project's EXISTING sanitizer machinery instead of hand-rolled flags: `-DYARS_ENABLE_SANITIZERS=ON` (`cmake/SetOptions.cmake:60-62`, `cmake/SetCompilerSettings.cmake:47-59`) applies `-fsanitize=address,undefined,leak -fno-omit-frame-pointer` to C, CXX, and EXE/SHARED/MODULE linker flags — the MODULE coverage matters because controller plugins are dlopen'd. Also pass `-DYARS_BUILD_TESTS=OFF` to skip a sanitized GTest build.

```yaml
  sanitize:
    name: ubuntu-22.04 / ASan+UBSan+LSan headless corpus
    runs-on: ubuntu-22.04
    timeout-minutes: 90
    steps:
      - name: Checkout (with submodules)
        uses: actions/checkout@v6
        with:
          submodules: recursive

      - name: Install apt dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y \
            build-essential cmake git pkg-config ninja-build \
            libcli11-dev libbullet-dev libsdl2-dev libxerces-c-dev \
            libfreetype6-dev libzzip-dev zlib1g-dev

      - name: Configure sanitized build
        run: |
          cmake -S . -B build-san \
            -DCMAKE_BUILD_TYPE=RelWithDebInfo \
            -DYARS_USE_VISUALISATION=OFF \
            -DYARS_BUILD_TESTS=OFF \
            -DYARS_ENABLE_SANITIZERS=ON

      - name: Build sanitized YARS
        run: cmake --build build-san -j 4

      - name: Run sanitized corpus
        run: ./scripts/sanitize-corpus.sh build-san

      - name: Upload sanitizer logs on failure
        if: failure()
        uses: actions/upload-artifact@v7
        with:
          name: sanitizer-logs
          path: build-san/san-*.log
          retention-days: 14
```

- [ ] **Step 2: Validate YAML**

```bash
python3 -c "import yaml; yaml.safe_load(open('.github/workflows/linux-build.yml')); print('YAML OK')"
```

- [ ] **Step 3: Commit**

```bash
git add .github/workflows/linux-build.yml
git commit -m "ci: Linux ASan+UBSan+LSan job over headless corpus — delivers the leak check deferred in the May 2026 audit"
```

---

### Task 5: Performance measurement + trend scripts

**Files:**
- Create: `scripts/perf-measure.sh`
- Create: `scripts/perf-trend-check.sh`

**Interfaces:**
- Produces: `perf-measure.sh <build-dir>` prints exactly one line `<unix-ts> <git-sha> <platform> <steps_per_sec>` to stdout. `perf-trend-check.sh <history-file> <new-steps-per-sec>` exits 0 always, prints `::warning::...` iff the new value is >25% below the rolling median of the last 20 entries.

- [ ] **Step 1: Write perf-measure.sh**

```bash
#!/usr/bin/env bash
# Times a 100k-iteration headless braitenberg run; prints one history line.
# Usage: scripts/perf-measure.sh <build-dir>
# Portable to macOS: BSD date has no %N and macOS ships no GNU timeout,
# so timing uses python3 and the timeout wrapper is optional.
set -eu
BUILD_DIR="${1:?usage: perf-measure.sh <build-dir>}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ITER=100000
TIMEOUT_BIN=$(command -v timeout || command -v gtimeout || true)
now() { python3 -c 'import time; print(time.time())'; }
cd "$BUILD_DIR"
START=$(now)
${TIMEOUT_BIN:+$TIMEOUT_BIN 600s} ./bin/yars --iterations $ITER --nogui \
  --xml "$ROOT/xml/braitenberg.xml" > /dev/null 2>&1
END=$(now)
SPS=$(echo "$ITER $END $START" | awk '{printf "%.0f", $1/($2-$3)}')
SHA=$(git -C "$ROOT" rev-parse --short HEAD)
PLATFORM="$(uname -s)-$(uname -m)"
echo "$(date +%s) $SHA $PLATFORM $SPS"
```

Notes: braitenberg.xml needs `libYarsControllerBraitenberg2b`; the script runs from the build dir so the cwd/lib fallback resolves it (same pattern as CI). The 600s cap is generous but unverified on ubuntu runners — during Task 6's first CI run, check the step's actual duration and lower `ITER` to 50000 if it exceeds ~5 minutes.

- [ ] **Step 2: Write perf-trend-check.sh**

```bash
#!/usr/bin/env bash
# Usage: perf-trend-check.sh <history-file> <new-steps-per-sec>
# Warns (never fails) when new value is >25% below the median of the
# last 20 entries. Fewer than 5 entries: no judgement possible.
set -eu
HIST="${1:?history file}"; NEW="${2:?new steps/s}"
if [ ! -f "$HIST" ] || [ "$(wc -l < "$HIST")" -lt 5 ]; then
  echo "perf-trend: <5 history entries, no baseline yet (new: ${NEW} steps/s)"
  exit 0
fi
MEDIAN=$(tail -20 "$HIST" | awk '{print $4}' | sort -n | awk '{a[NR]=$1} END{print (NR%2? a[(NR+1)/2] : (a[NR/2]+a[NR/2+1])/2)}')
THRESH=$(echo "$MEDIAN" | awk '{printf "%.0f", $1*0.75}')
if [ "$NEW" -lt "$THRESH" ]; then
  echo "::warning::Performance drop: ${NEW} steps/s vs rolling median ${MEDIAN} (threshold ${THRESH})"
else
  echo "perf-trend: OK (${NEW} steps/s, median ${MEDIAN})"
fi
exit 0
```

```bash
chmod +x scripts/perf-measure.sh scripts/perf-trend-check.sh
```

- [ ] **Step 3: Test locally (this is the test cycle)**

```bash
cd /Volumes/Eregion/projects/yars
./scripts/perf-measure.sh build
# Expected: one line like "1751808000 dae130f Darwin-arm64 42000"
./scripts/perf-measure.sh build > /tmp/hist.txt
for i in 1 2 3 4; do ./scripts/perf-measure.sh build >> /tmp/hist.txt; done
LAST=$(tail -1 /tmp/hist.txt | awk '{print $4}')
./scripts/perf-trend-check.sh /tmp/hist.txt "$LAST"          # expect: perf-trend: OK
./scripts/perf-trend-check.sh /tmp/hist.txt 100              # expect: ::warning::Performance drop
./scripts/perf-trend-check.sh /tmp/hist.txt 100; echo "exit=$?"  # expect: exit=0 (warn-only)
```

- [ ] **Step 4: Commit**

```bash
git add scripts/perf-measure.sh scripts/perf-trend-check.sh
git commit -m "ci: perf measurement + warn-only trend check scripts"
```

---

### Task 6: Perf trend steps in both workflows + history branch

**Files:**
- Modify: `.github/workflows/linux-build.yml` (steps at the end of `build-and-audit`)
- Modify: `.github/workflows/macos-build.yml` (same, at end of its build job)

**Interfaces:**
- Consumes: `scripts/perf-measure.sh`, `scripts/perf-trend-check.sh` (Task 5).
- Produces: history files `perf/<platform>.log` on branch `ci-perf-history`.

- [ ] **Step 1: Bootstrap the history branch (one-time, local)**

Never run `git checkout --orphan` in the main worktree (destructive to working files). Use a throwaway worktree:

```bash
cd /Volumes/Eregion/projects/yars
git worktree add "$SCRATCH/yars-perf-history" --detach   # SCRATCH = the session scratchpad dir, not /tmp
cd "$SCRATCH/yars-perf-history"
git checkout --orphan ci-perf-history
git rm -rf --cached . >/dev/null 2>&1 || true
git clean -fdx >/dev/null 2>&1 || true
mkdir -p perf
printf "# ts sha platform steps_per_sec\n" > perf/README
git add perf/README
git commit -m "ci: bootstrap perf history branch"
cd /Volumes/Eregion/projects/yars
git worktree remove "$SCRATCH/yars-perf-history"
```

Do NOT push the branch yet — pushing requires user approval; note it as a handoff item.

- [ ] **Step 2: Add the CI steps (identical block, both workflows, end of build job)**

Both workflows also need write permission for the history push — add at the JOB level of the build job in each workflow (workflow files currently declare no `permissions:` at all, so the org/repo default may be read-only and the push would 403):

```yaml
    permissions:
      contents: write
```

The trend step itself (`continue-on-error` keeps the spec's warn-only promise even if the measurement itself breaks):

```yaml
      - name: Performance trend (warn-only)
        continue-on-error: true
        env:
          LD_LIBRARY_PATH: ${{ env.OGRE_INSTALL_PREFIX }}/lib
        run: |
          LINE=$(./scripts/perf-measure.sh build)
          echo "measured: $LINE"
          PLATFORM=$(echo "$LINE" | awk '{print $3}')
          SPS=$(echo "$LINE" | awk '{print $4}')
          git fetch origin ci-perf-history:ci-perf-history || {
            echo "::warning::ci-perf-history branch not on origin yet; skipping trend check"; exit 0; }
          git worktree add ../perf-history ci-perf-history
          HIST="../perf-history/perf/${PLATFORM}.log"
          touch "$HIST"
          ./scripts/perf-trend-check.sh "$HIST" "$SPS"
          echo "$LINE" >> "$HIST"
          cd ../perf-history
          git config user.name "yars-ci"
          git config user.email "ci@users.noreply.github.com"
          git add perf/ && git commit -m "perf: ${PLATFORM} $(echo "$LINE" | awk '{print $2, $4}')" || true
          # Linux and macOS jobs run on the same push and race on this
          # branch; rebase-retry so neither platform's point is lost.
          for i in 1 2 3; do
            git push origin ci-perf-history && break
            git pull --rebase origin ci-perf-history || true
            [ "$i" = "3" ] && echo "::warning::perf-history push failed after retries"
          done
```

On macOS: drop the `env:` block (the macOS workflow's run steps use none) — everything else is identical.

- [ ] **Step 3: Validate YAML and commit**

```bash
python3 -c "import yaml; yaml.safe_load(open('.github/workflows/linux-build.yml')); yaml.safe_load(open('.github/workflows/macos-build.yml')); print('YAML OK')"
git add .github/workflows/
git commit -m "ci: warn-only perf trend recorded on ci-perf-history branch"
```

---

### Task 7: End-to-end verification and reference landing (needs user for pushes)

**Files:** none new; verification + two follow-up commits.

- [ ] **Step 1: Hand off for push approval.** Pushing `feat/ci-quality-gates` and `ci-perf-history` requires user approval (project rule). Request it, then push both.

- [ ] **Step 2: After CI runs:** download the two hexapod reference-candidate artifacts from the workflow run, commit them:

```bash
git add xml/reference_logfile_hexapod.linux-x86_64.csv xml/reference_logfile_hexapod.macos-arm64.csv
git commit -m "test(xml): CI-generated hexapod reference logfiles (both platforms)"
```

Push (with approval) and confirm the next run's hexapod steps report "byte-identical".

- [ ] **Step 3: Sanitizer canary.** On a scratch branch off this one, add a deliberate leak to `src/yars/yarsMain.cpp`. CORRECTED 2026-07-06 after three failed constructions — a valid LSan canary must satisfy BOTH properties:
  1. **Escape** (or GCC's heap elision at -O2 removes the allocation entirely — a function-local pointer that never escapes compiles to no allocation);
  2. **Unreachable at exit** (LSan's conservative scan treats any pointer still visible in a live stack frame, register, or global as reachable and correctly does NOT report it — so a `main()`-local or a still-set global hides the leak).

```cpp
int *g_lsan_canary_sink = nullptr;           // escape target (LSan root)
static void lsanCanary(int seed) {
  g_lsan_canary_sink = new int[10];          // escapes via global: no elision
  g_lsan_canary_sink[0] = seed;
  std::cout << "lsan-canary " << g_lsan_canary_sink[0] << std::endl;
  g_lsan_canary_sink = nullptr;              // sever the only root: unreachable
}
// call lsanCanary(argc) at the top of main()
```

Push (with approval) and confirm the `sanitize` job FAILS with a LeakSanitizer report naming `lsanCanary`. Delete the scratch branch afterwards. This proves the gate bites end-to-end — which matters more than it sounds: the first canary campaign exposed that YARS's SIGABRT handler converted sanitizer aborts into exit(0) (fixed in `42e008f`), a hole no green run could ever reveal.

- [ ] **Step 4: Verify GUI still works locally (project completion bar)**

```bash
cd /Volumes/Eregion/projects/yars/build
timeout 60s ./bin/yars --iterations 500 --xml ../xml/braitenberg.xml
```

Expected: window opens, robots render and move, clean exit at iteration 500.
