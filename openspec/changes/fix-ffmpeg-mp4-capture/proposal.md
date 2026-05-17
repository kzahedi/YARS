# Fix FFmpeg .mp4 capture sometimes producing no output

## Why

The Linux CI workflow's "FFmpeg video capture (xvfb + llvmpipe)" step is
warning-level:

```
xvfb-run -a -s "-screen 0 1920x1080x24" \
  ./build/bin/yars --iterations 1800 --xml xml/test_capture.xml ...
rc=$?
MP4=$(ls -t *.mp4 2>/dev/null | head -1)
if [ -n "$MP4" ]; then
  mv "$MP4" build/yars-capture.mp4
  ...
else
  echo "::warning::no .mp4 produced (rc=$rc); check video-capture.log"
fi
```

`yars` exits cleanly (rc=0) but on some runs no `.mp4` lands in the
working directory. Today this only emits a `::warning::` and the job
continues. That means a future regression that breaks video capture
end-to-end won't fail CI — it just silently keeps shipping the same
warning.

We want CI to either:

1. Reliably produce the .mp4 (i.e., fix whatever races / timing /
   plugin-load issue causes the miss), or
2. Distinguish "capture not attempted" from "capture failed" so the
   workflow can fail on the latter without false positives on the
   former.

## What changes

Investigation first, then either a code fix or a workflow gate. Pieces
that may need to change once root cause is known:

1. **`xml/test_capture.xml`** — does it actually request capture? Is
   the `<capture>` element present and pointing at a writable path?
2. **`src/yars/view/gui/SdlWindow.cpp`** + capture wiring in
   `YarsMainControl` — frame capture is per-window; under xvfb +
   llvmpipe the first frame might race ahead of the capture-encoder
   init. Need to confirm whether the encoder is given enough warmup
   frames.
3. **FFmpeg encoder threading** — Ogre + xvfb under llvmpipe is
   single-threaded. Capture encode may be racing GUI shutdown when
   `--iterations` is small. The current run uses 1800 iterations
   which should be plenty, but the timing under software rendering
   is much slower than native.
4. **Workflow** — if root cause is "sometimes the encoder doesn't
   flush before exit", the fix may need both a code-side flush guard
   and a longer wait in CI. If root cause is environmental, the
   workflow can grow a `set -e`-equivalent gate.

## Non-goals

- Investigating macOS capture path (separate environment, different
  encoder pipeline, not currently covered by CI).
- Adding video diffing / pixel comparison against a reference. Out
  of scope; this proposal only enforces "an mp4 was produced".

## Open questions

- Is the missing-mp4 case intermittent or deterministic on the same
  runner image? Need to check the run history (the warning has been
  in the artifact summaries for weeks).
- Does the same test config produce output reliably on macOS CI?
