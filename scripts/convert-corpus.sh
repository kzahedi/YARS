#!/usr/bin/env bash
#
# Runs `yars --convert` over every XML config in the corpus (xml/*.xml and
# xml/joints/*.xml) and reports PASS/FAIL per file.
#
# Only genuine converter bugs are expected to fail conversion now. Prior to
# Stage 1's #children amendment (see src/yars/configuration/json/XmlToJson.h
# and docs/planning/json-migration-notes.md), non-contiguous same-tag
# siblings under a parent (e.g. hexaboard.xml/hexapod_*.xml's interleaved
# <deflection> sensors, muscle_tcpip.xml's interleaved <domain>/<mapping>)
# made the converter throw loudly. XmlToJson now represents those parents'
# children as an ordered "#children" array instead, so all corpus files are
# expected to convert. EXPECTED_FAIL stays empty; only add an entry after
# inspecting a NEW failure and confirming it is a genuine unhandled case
# (e.g. non-whitespace element text content), not a converter regression.
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

EXPECTED_FAIL=()

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
