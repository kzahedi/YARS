# Tasks

## 1. Reproduce + diagnose

- [ ] 1.1 Pull the last 20 Linux CI runs and tabulate which produced
      `yars-capture.mp4` vs which only have `video-capture.log`. If
      it's 100% missing, focus shifts to "capture was never wired";
      if it's intermittent, focus shifts to a race.
- [ ] 1.2 Read `xml/test_capture.xml` and confirm what it asks for
      (screen show=true, capture flag, frame rate, output path).
      Document what a successful run *should* produce.
- [ ] 1.3 Capture `video-capture.log` from a failing run (download
      the `linux-audit-artifacts` zip from a recent run) and look
      for FFmpeg / encoder errors or "no frames received".
- [ ] 1.4 If reproducible locally, run under `xvfb-run` + llvmpipe
      on macOS or in a Linux container to bisect.

## 2. Fix or gate

- [ ] 2.1 If a code-level fix is required: most likely
      `SdlWindow::stopCaptureVideo()` or the encoder destructor isn't
      flushing the trailing frames before SDL_QuitSubsystem. Add an
      explicit flush + close on the encoder side.
- [ ] 2.2 If the issue is xvfb timing: extend the xvfb startup wait
      in the workflow (`xvfb-run --auto-display ... --wait-for-x`).
- [ ] 2.3 If the .mp4 is named differently than expected (e.g.,
      written under `build/` rather than `cwd`), the workflow's
      `ls *.mp4` glob is wrong — fix the glob to match.

## 3. CI enforcement

- [ ] 3.1 Once capture is reliable, change the workflow step from
      `::warning::` to a hard fail when `MP4` is empty *and* the
      test XML asked for capture. If the XML didn't ask, skip
      silently.
- [ ] 3.2 Upload the produced `.mp4` as an artifact (already done)
      and capture an `ffprobe` summary (already done) so future
      regressions in frame count / codec are visible.

## 4. Documentation

- [ ] 4.1 If we land a code fix, note it in `CLAUDE.md` under the
      capture pipeline section.
- [ ] 4.2 Cross off the FFmpeg item in
      `docs/planning/v0.8.7-open-points.md`.
