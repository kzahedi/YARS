#!/usr/bin/env bash
#
# Stage 1 acceptance check (JSON Stage 1 brief, Step 4): for each config in
# the CI-runnable corpus (the same 13 configs sanitize-corpus.sh exercises,
# mirroring the STANDALONE/CFG2LIB arrays in .github/workflows/linux-build.yml),
# convert XML -> JSON, run both from the .xml and from the converted .json,
# and compare:
#   - braitenberg_logging / hexapod_logging (the two logging configs):
#     bit-exact CSV diff.
#   - everything else: exit code + final console line.
#
# Usage: scripts/json-roundtrip-check.sh <build-dir>   (default: build)

set -u

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="${1:-$ROOT/build}"
# Resolve to an absolute path: run_one() cd's into a scratch directory
# before invoking $YARS_BIN, so a relative BUILD_DIR (e.g. the default
# caller-friendly "build") would otherwise silently fail to resolve there.
BUILD_DIR="$(cd "$BUILD_DIR" && pwd)"
YARS_BIN="$BUILD_DIR/bin/yars"

if [[ ! -x "$YARS_BIN" ]]; then
  echo "Error: $YARS_BIN not found or not executable. Build first." >&2
  exit 1
fi

# Standalone configs (no controller library needed).
STANDALONE=(
  xml/braitenberg_nocontroller.xml
  xml/falling_objects.xml
  xml/test_capture.xml
  xml/hexapod_logging.xml
)
# Controller-based configs: config -> lib name (mirrors sanitize-corpus.sh /
# linux-build.yml).
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

# The two logging configs get a bit-exact CSV diff; everything else gets
# exit-code + final-console-line comparison.
is_logging_config() {
  case "$1" in
    xml/braitenberg_logging.xml|xml/hexapod_logging.xml) return 0 ;;
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
  name="$(basename "$cfg" .xml)"
  TOTAL=$((TOTAL + 1))

  local xmlpath="$ROOT/$cfg"
  local jsonpath="${xmlpath%.xml}.json"

  # 1. Convert.
  if ! timeout 30s "$YARS_BIN" --convert "$xmlpath" >"$WORKDIR/${name}-convert.log" 2>&1; then
    echo "FAIL $cfg (conversion failed, see $WORKDIR/${name}-convert.log)"
    sed 's/^/  /' "$WORKDIR/${name}-convert.log"
    FAILED=1
    return
  fi
  if [[ ! -f "$jsonpath" ]]; then
    echo "FAIL $cfg (conversion did not produce $jsonpath)"
    FAILED=1
    return
  fi

  # 2/3. Run 500 iters from each format. Controller libraries are located
  # via a path relative to CWD (confirmed: running from outside $BUILD_DIR
  # fails with "Controller '...' not found"), so both runs must execute
  # with $BUILD_DIR as CWD, exactly like sanitize-corpus.sh does. CSV
  # logging output also lands in CWD, so any newly-created *.csv is moved
  # out to a per-run scratch dir immediately after each run (sequential,
  # not parallel, so there's no cross-run collision).
  local xmldir="$WORKDIR/${name}-xml"
  local jsondir="$WORKDIR/${name}-json"
  mkdir -p "$xmldir" "$jsondir"

  local before_csvs after_csvs new_csv
  before_csvs="$(ls "$BUILD_DIR"/*.csv 2>/dev/null || true)"
  ( cd "$BUILD_DIR" && timeout 120s "$YARS_BIN" --iterations 500 --nogui --xml "$xmlpath" \
      >"$WORKDIR/${name}-xml.log" 2>&1 )
  local xml_rc=$?
  after_csvs="$(ls "$BUILD_DIR"/*.csv 2>/dev/null || true)"
  for f in $after_csvs; do
    if ! grep -qxF "$f" <<<"$before_csvs"; then
      mv "$f" "$xmldir/"
    fi
  done

  before_csvs="$(ls "$BUILD_DIR"/*.csv 2>/dev/null || true)"
  ( cd "$BUILD_DIR" && timeout 120s "$YARS_BIN" --iterations 500 --nogui --xml "$jsonpath" \
      >"$WORKDIR/${name}-json.log" 2>&1 )
  local json_rc=$?
  after_csvs="$(ls "$BUILD_DIR"/*.csv 2>/dev/null || true)"
  for f in $after_csvs; do
    if ! grep -qxF "$f" <<<"$before_csvs"; then
      mv "$f" "$jsondir/"
    fi
  done

  if [[ $xml_rc -ne $json_rc ]]; then
    echo "FAIL $cfg (exit code mismatch: xml=$xml_rc json=$json_rc)"
    FAILED=1
    return
  fi

  if is_logging_config "$cfg"; then
    local xml_csv json_csv
    xml_csv=$(ls "$xmldir"/*.csv 2>/dev/null | head -1)
    json_csv=$(ls "$jsondir"/*.csv 2>/dev/null | head -1)
    if [[ -z "$xml_csv" || -z "$json_csv" ]]; then
      echo "FAIL $cfg (expected CSV logging output, xml_csv='$xml_csv' json_csv='$json_csv')"
      FAILED=1
      return
    fi
    if ! diff -q "$xml_csv" "$json_csv" >/dev/null; then
      echo "FAIL $cfg (CSV mismatch: $xml_csv vs $json_csv)"
      diff "$xml_csv" "$json_csv" | head -20
      FAILED=1
      return
    fi
    echo "PASS $cfg (exit=$xml_rc, CSV bit-exact)"
  else
    local xml_last json_last
    xml_last=$(tail -1 "$WORKDIR/${name}-xml.log")
    json_last=$(tail -1 "$WORKDIR/${name}-json.log")
    if [[ "$xml_last" != "$json_last" ]]; then
      echo "FAIL $cfg (final console line mismatch)"
      echo "  xml : $xml_last"
      echo "  json: $json_last"
      FAILED=1
      return
    fi
    echo "PASS $cfg (exit=$xml_rc, final line matches)"
  fi
  PASSED=$((PASSED + 1))
  rm -f "$jsonpath"
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
echo "Round-trip: $PASSED/$TOTAL passed"

exit $FAILED
