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
