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
