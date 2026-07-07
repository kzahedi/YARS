#!/usr/bin/env bash
#
# JSON self-check gate (formerly the JSON Stage 1 XML<->JSON round-trip
# check; retargeted for JSON Stage 3, the XML/Xerces harvest — see
# v0.9.0-last-xml for the last release that could still run this check
# against a live --convert / XML-SAX path).
#
# There is no XML path left to diff against, so this now runs each config
# in the CI-runnable corpus (the same set sanitize-corpus.sh exercises,
# mirroring the STANDALONE/CFG2LIB arrays in .github/workflows/linux-build.yml)
# TWICE from its committed .json and checks for determinism:
#   - braitenberg_logging / hexapod_logging (the two logging configs):
#     bit-exact CSV diff between the two runs.
#   - everything else: exit code 0 on both runs + identical final console
#     line between the two runs.
#
# Usage: scripts/json-roundtrip-check.sh <build-dir>   (default: build)

set -u

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${1:-$ROOT/build}"
BUILD_DIR="$(cd "$BUILD_DIR" && pwd)"
YARS_BIN="$BUILD_DIR/bin/yars"

if [[ ! -x "$YARS_BIN" ]]; then
  echo "Error: $YARS_BIN not found or not executable. Build first." >&2
  exit 1
fi

# Standalone configs (no controller library needed).
STANDALONE=(
  xml/braitenberg_nocontroller.json
  xml/falling_objects.json
  xml/test_capture.json
  xml/hexapod_logging.json
)
# Controller-based configs: config -> lib name (mirrors sanitize-corpus.sh /
# linux-build.yml).
CONFIGS=(
  "xml/braitenberg.json:YarsControllerBraitenberg2b"
  "xml/braitenberg_noise.json:YarsControllerBraitenberg2b"
  "xml/braitenberg_logging.json:YarsControllerBraitenberg3b"
  "xml/braitenberg_light_source.json:YarsControllerBraitenberg2b"
  "xml/braitenberg_trace_projection.json:YarsControllerBraitenberg2b"
  "xml/braitenberg_zoo.json:YarsControllerBraitenberg2a"
  "xml/muscle.json:YarsControllerSquareWave"
  "xml/joints/generic_angular.json:YarsControllerSine"
  "xml/joints/generic_force.json:YarsControllerSine"
)

is_logging_config() {
  case "$1" in
    xml/braitenberg_logging.json|xml/hexapod_logging.json) return 0 ;;
    *) return 1 ;;
  esac
}

WORKDIR="$(mktemp -d)"
trap 'rm -rf "$WORKDIR"' EXIT

FAILED=0
TOTAL=0
PASSED=0

run_one() {
  local cfg="$1"
  local name
  name="$(basename "$cfg" .json)"
  TOTAL=$((TOTAL + 1))

  local jsonpath="$ROOT/$cfg"
  local rundir_a="$WORKDIR/${name}-a"
  local rundir_b="$WORKDIR/${name}-b"
  mkdir -p "$rundir_a" "$rundir_b"

  # Run the same committed .json twice and require deterministic output.
  # Controller libraries are located via a path relative to CWD (confirmed:
  # running from outside $BUILD_DIR fails with "Controller '...' not
  # found"), so both runs execute with $BUILD_DIR as CWD, exactly like
  # sanitize-corpus.sh does. CSV logging output also lands in CWD, so any
  # newly-created *.csv is moved out to a per-run scratch dir immediately
  # after each run (sequential, not parallel, so there's no cross-run
  # collision).
  local before_csvs after_csvs
  before_csvs="$(ls "$BUILD_DIR"/*.csv 2>/dev/null || true)"
  ( cd "$BUILD_DIR" && timeout 120s "$YARS_BIN" --iterations 500 --nogui --xml "$jsonpath" \
      >"$WORKDIR/${name}-a.log" 2>&1 )
  local rc_a=$?
  after_csvs="$(ls "$BUILD_DIR"/*.csv 2>/dev/null || true)"
  for f in $after_csvs; do
    if ! grep -qxF "$f" <<<"$before_csvs"; then
      mv "$f" "$rundir_a/"
    fi
  done

  before_csvs="$(ls "$BUILD_DIR"/*.csv 2>/dev/null || true)"
  ( cd "$BUILD_DIR" && timeout 120s "$YARS_BIN" --iterations 500 --nogui --xml "$jsonpath" \
      >"$WORKDIR/${name}-b.log" 2>&1 )
  local rc_b=$?
  after_csvs="$(ls "$BUILD_DIR"/*.csv 2>/dev/null || true)"
  for f in $after_csvs; do
    if ! grep -qxF "$f" <<<"$before_csvs"; then
      mv "$f" "$rundir_b/"
    fi
  done

  if [[ $rc_a -ne 0 || $rc_b -ne 0 ]]; then
    echo "FAIL $cfg (non-zero exit: run-a=$rc_a run-b=$rc_b)"
    FAILED=1
    return
  fi

  if is_logging_config "$cfg"; then
    local csv_a csv_b
    csv_a=$(ls "$rundir_a"/*.csv 2>/dev/null | head -1)
    csv_b=$(ls "$rundir_b"/*.csv 2>/dev/null | head -1)
    if [[ -z "$csv_a" || -z "$csv_b" ]]; then
      echo "FAIL $cfg (expected CSV logging output, run-a='$csv_a' run-b='$csv_b')"
      FAILED=1
      return
    fi
    if ! diff -q "$csv_a" "$csv_b" >/dev/null; then
      echo "FAIL $cfg (CSV mismatch between two runs: $csv_a vs $csv_b)"
      diff "$csv_a" "$csv_b" | head -20
      FAILED=1
      return
    fi
    echo "PASS $cfg (exit=0, CSV bit-exact across two runs)"
  else
    local last_a last_b
    last_a=$(tail -1 "$WORKDIR/${name}-a.log")
    last_b=$(tail -1 "$WORKDIR/${name}-b.log")
    if [[ "$last_a" != "$last_b" ]]; then
      echo "FAIL $cfg (final console line mismatch between two runs)"
      echo "  run-a: $last_a"
      echo "  run-b: $last_b"
      FAILED=1
      return
    fi
    echo "PASS $cfg (exit=0, final line matches across two runs)"
  fi
  PASSED=$((PASSED + 1))
}

for cfg in "${STANDALONE[@]}"; do
  run_one "$cfg"
done
for entry in "${CONFIGS[@]}"; do
  cfg="${entry%%:*}"
  lib="${entry##*:}"
  if ! ls "$BUILD_DIR"/lib/lib${lib}.* >/dev/null 2>&1; then
    echo "FAIL $cfg (lib${lib} missing from $BUILD_DIR/lib — build first)"
    FAILED=1
    TOTAL=$((TOTAL + 1))
    continue
  fi
  run_one "$cfg"
done

echo ""
echo "JSON self-check: $PASSED/$TOTAL passed"

exit $FAILED
