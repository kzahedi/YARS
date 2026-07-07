#!/usr/bin/env bash
# Runs the CI-runnable config corpus under a sanitized YARS build.
# Usage: scripts/sanitize-corpus.sh <sanitized-build-dir>
# Any sanitizer report (halt_on_error=1) makes the run exit non-zero.
set -u
BUILD_DIR="${1:?usage: sanitize-corpus.sh <build-dir>}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SUPP="$ROOT/scripts/sanitizer-suppressions.txt"

# NO abort_on_error: sanitizer failures must exit via the runtime's own
# non-zero exit code, not SIGABRT — an abort raised inside the process
# would historically be converted to exit(0) by YARS's signal handling
# and read as PASS (root-caused 2026-07-06 via the LSan canary).
export ASAN_OPTIONS="halt_on_error=1:detect_leaks=1"
export UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1"
export LSAN_OPTIONS="suppressions=$SUPP"

# Standalone configs (no controller library needed)
STANDALONE=(
  xml/braitenberg_nocontroller.json
  xml/falling_objects.json
  xml/test_capture.json
  xml/hexapod_logging.json
)
# Controller-based configs: config -> lib name (mirrors linux-build.yml)
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

FAILED=0
cd "$BUILD_DIR"
for cfg in "${STANDALONE[@]}"; do
  name=$(basename "$cfg" .json)
  # Sanitized builds are ~2-4x slower: 240s cap, 500 iterations
  if timeout 240s ./bin/yars --iterations 500 --nogui --xml "$ROOT/$cfg" > "san-${name}.log" 2>&1; then
    echo "PASS $cfg"
  else
    echo "FAIL $cfg (see $BUILD_DIR/san-${name}.log)"; FAILED=1
  fi
done
for entry in "${CONFIGS[@]}"; do
  cfg="${entry%%:*}"; lib="${entry##*:}"
  name=$(basename "$cfg" .json)
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
