#!/usr/bin/env bash
#
# Runs `yars --convert` over every XML config in the corpus (xml/*.xml and
# xml/joints/*.xml) and reports PASS/FAIL per file.
#
# Some corpus files are expected to legitimately fail conversion, per the
# XmlToJson converter's loud-failure design (see
# src/yars/configuration/json/XmlToJson.h): non-whitespace element text
# content, or non-contiguous same-tag siblings under a parent. Those files
# are listed in EXPECTED_FAIL below, each with a comment citing the specific
# element responsible.
#
# Usage: ./scripts/convert-corpus.sh   (run from repo root)

set -u

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$REPO_ROOT" || exit 1

YARS_BIN="./build/bin/yars"

if [[ ! -x "$YARS_BIN" ]]; then
  echo "Error: $YARS_BIN not found or not executable. Build first (see build/ instructions)." >&2
  exit 1
fi

# Files that are expected to fail --convert. Empty initially; add entries
# here only after inspecting the failure and confirming it is a genuine
# interleaved-siblings or text-content case, not a converter bug.
EXPECTED_FAIL=(
  # <sensors> mixes deflection/deflection/binaryContact groups repeatedly
  # (e.g. lines ~513-572): <deflection> siblings are non-contiguous.
  "xml/hexaboard.xml"
  # same <sensors> interleaving pattern as hexaboard.xml.
  "xml/hexapod_logging.xml"
  # same <sensors> interleaving pattern as hexaboard.xml.
  "xml/hexapod_mpi.xml"
  # <actuators> has <generic> blocks (lines ~294-412), then <hinge> blocks
  # (~414-531), then more <generic> blocks (~533+): <generic> siblings are
  # non-contiguous.
  "xml/hexapod_ralf.xml"
  # same <sensors>/<deflection> interleaving pattern as hexaboard.xml.
  "xml/hexapod_shadow_test.xml"
  # <muscle> alternates <domain>/<mapping> pairs repeatedly (lines ~123-136):
  # <domain> (and <mapping>) siblings are non-contiguous.
  "xml/muscle_tcpip.xml"
)

is_expected_fail() {
  local f="$1"
  for ef in "${EXPECTED_FAIL[@]:-}"; do
    [[ "$f" == "$ef" ]] && return 0
  done
  return 1
}

shopt -s nullglob
files=(xml/*.xml xml/joints/*.xml)
shopt -u nullglob

total=0
passed=0
failed_unexpected=0
failed_expected=0

for f in "${files[@]}"; do
  total=$((total + 1))
  if timeout 30s "$YARS_BIN" --convert "$f" >/tmp/convert-corpus.out 2>&1; then
    echo "PASS: $f"
    passed=$((passed + 1))
  else
    if is_expected_fail "$f"; then
      echo "FAIL (expected): $f"
      failed_expected=$((failed_expected + 1))
    else
      echo "FAIL (unexpected): $f"
      sed 's/^/  /' /tmp/convert-corpus.out
      failed_unexpected=$((failed_unexpected + 1))
    fi
  fi
done
rm -f /tmp/convert-corpus.out

echo ""
echo "Total: $total, converted: $passed, expected-fail: $failed_expected, unexpected-fail: $failed_unexpected"

if [[ $failed_unexpected -gt 0 ]]; then
  exit 1
fi

exit 0
