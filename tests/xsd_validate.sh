#!/usr/bin/env bash
# XSD validation driver: walks tests/xml_corpus.txt and validates each entry
# against xml/yars.xsd using xmllint.
#
# Exit codes:
#   0 — all in-scope configs validated cleanly
#   1 — at least one validation failure (full xmllint output printed)
#   2 — environment issue (xmllint missing or schema/corpus not found)
#
# Usage:
#   tests/xsd_validate.sh [<repo-root>]
#
# The first positional arg may override the repo root (defaults to the script's
# parent directory). This makes the script callable from a build dir.

set -u
set -o pipefail

repo_root="${1:-$(cd "$(dirname "$0")/.." && pwd)}"
schema="$repo_root/xml/yars.xsd"
corpus="$repo_root/tests/xml_corpus.txt"

if ! command -v xmllint >/dev/null 2>&1; then
  echo "ERROR: xmllint not found in PATH. Install via:" >&2
  echo "  macOS:  brew install libxml2  (then add to PATH)" >&2
  echo "  Linux:  apt-get install libxml2-utils  (or distro equivalent)" >&2
  exit 2
fi

if [[ ! -f "$schema" ]]; then
  echo "ERROR: schema not found at $schema" >&2
  echo "Regenerate with: yars --export xsd && mv rosiml.xsd $schema" >&2
  exit 2
fi

if [[ ! -f "$corpus" ]]; then
  echo "ERROR: corpus list not found at $corpus" >&2
  exit 2
fi

pass=0
fail=0
failed=()

while IFS= read -r rel; do
  [[ -z "$rel" || "${rel:0:1}" == "#" ]] && continue
  abs="$repo_root/$rel"
  if [[ ! -f "$abs" ]]; then
    echo "MISSING $rel"
    failed+=("$rel: file not found")
    fail=$((fail + 1))
    continue
  fi

  if output=$(xmllint --noout --schema "$schema" "$abs" 2>&1); then
    pass=$((pass + 1))
    echo "PASS    $rel"
  else
    fail=$((fail + 1))
    failed+=("$rel")
    echo "FAIL    $rel"
    printf '%s\n' "$output" | sed 's/^/    /'
  fi
done < "$corpus"

echo
echo "Summary: $pass passed, $fail failed"
if (( fail > 0 )); then
  printf '\nFailed configs:\n'
  printf '  - %s\n' "${failed[@]}"
  exit 1
fi
exit 0
