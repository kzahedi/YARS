# Tasks

## 1. Reproduce + diagnose

- [x] 1.1 Downloaded the last successful master run
      (`25991391662`) and inspected its
      `linux-audit-logs/video-capture.log`. Found
      `InternalErrorException: No render systems available in
      OgreHandler at OgreHandler.cpp (line 44)` — the GUI was never
      starting, so no frames were rendered, hence no .mp4.
- [x] 1.2 `xml/test_capture.xml` requests `<screen capture="true"
      show="true">` but that XML attribute is only for per-screen
      activation. The actual encoder start needs the `--capture`
      CLI flag *and* the `YARS_USE_CAPTURE_VIDEO` CMake option.
- [x] 1.3 Iteratively chased seven distinct root causes (see
      section 2). Each fix uncovered the next layer because the
      previous workflow swallowed errors via `set +e` + ::warning::.

## 2. Fixes (the chain)

- [x] 2.1 **cwd-relative plugins.cfg** — `Ogre::Root("plugins.cfg",
      "ogre.cfg", "")` in `OgreHandler.cpp` looks up plugins.cfg in
      the cwd. The audit corpus steps `cd build` first; the capture
      step ran from the repo root and got an empty render-system
      list. Workflow `cd build` before xvfb-run. Commit `4a151af`.
- [x] 2.2 **Missing `--capture`** — without it, YarsViewModel's
      capture-init branch never runs. Added to the workflow.
      Commit `538eaef`.
- [x] 2.3 **`YARS_USE_CAPTURE_VIDEO=OFF` by default** —
      `cmake/SetOptions.cmake:48` defaults the option off, which
      strips `-DUSE_CAPTURE_VIDEO` from the build. The
      `#if USE_CAPTURE_VIDEO` guard in
      `YarsViewModel::toggleCaptureVideo` compiles out
      `startCaptureVideo()` per-window, so `--capture` is a no-op.
      Workflow now passes `-DYARS_USE_CAPTURE_VIDEO=ON`. Commit
      `50014d0`.
- [x] 2.4 **Missing apt deps** — once 2.3 enabled the capture path,
      cmake's `pkg_check_modules(FFMPEG REQUIRED libavformat
      libavcodec libavutil libavfilter libswscale libavdevice
      libswresample)` failed for `libavfilter` and `libavdevice`.
      Added `libavfilter-dev libavdevice-dev libavutil-dev` to the
      apt install line. Commit `c119c3b`.
- [x] 2.5 **FFmpeg 4 vs 5 ABI** — `VideoCapture.h` declared
      `const AVOutputFormat *oformat` to match Brew's FFmpeg 6.
      Ubuntu 22.04's libavformat 58 still has the non-const form.
      Branched on `LIBAVFORMAT_VERSION_MAJOR >= 59`. Commit
      `dd26550`.
- [x] 2.6 **Output file path** — defaults put the file at
      `video/yars.mp4` (relative to cwd); workflow looked in
      cwd directly. Pinned `--captureDirectory .` and
      `--captureName yars-capture.mp4`. Commit `63ea426`.
- [x] 2.7 **Segment-indexed filename** —
      `WindowConfiguration::getNextCaptureName` appends `-NNNN` to
      the basename. The actual produced file is
      `yars-capture-0001.mp4` even for a single segment. Workflow
      matches `yars-capture-*.mp4` and renames to the stable
      `yars-capture.mp4` for the ffprobe + artifact upload steps.
      Commit `345b8c1`.

## 3. CI enforcement

- [x] 3.1 Workflow step is now `set -e` with explicit `exit 1` on
      missing .mp4. The ::warning:: fallback is gone — a future
      regression that breaks capture will fail CI loudly.
- [x] 3.2 The produced `.mp4` is uploaded as part of
      `linux-audit-artifacts` along with `yars-capture-ffprobe.txt`
      so downstream regressions in frame count / codec / dimensions
      are visible from the artifact summary.

## 4. Documentation

- [ ] 4.1 No additional CLAUDE.md note needed — the seven-layer
      chain is captured in this tasks.md and in the commit messages.
- [x] 4.2 Cross off the FFmpeg item in
      `docs/planning/v0.8.7-open-points.md`.

## 5. Verification

- [x] 5.1 Final CI run `26020080771` produced a 600x600 H.264 mp4
      with 90+ frames (1800 sim iters at 50 fps capture rate).
      ffprobe artifact confirms codec_name=h264, codec_type=video,
      profile=High, codec_tag=avc1.
- [x] 5.2 Linux CI green; capture step now reliable.
