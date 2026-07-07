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
  --xml "$ROOT/xml/braitenberg.json" > /dev/null 2>&1
END=$(now)
SPS=$(echo "$ITER $END $START" | awk '{printf "%.0f", $1/($2-$3)}')
SHA=$(git -C "$ROOT" rev-parse --short HEAD)
PLATFORM="$(uname -s)-$(uname -m)"
echo "$(date +%s) $SHA $PLATFORM $SPS"
