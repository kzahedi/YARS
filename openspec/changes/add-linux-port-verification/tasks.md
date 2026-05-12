## 1. CI Workflow Skeleton

- [x] 1.1 Create `.github/workflows/linux-build.yml` triggering on push + pull_request, runs-on `ubuntu-22.04`
- [x] 1.2 Add `apt-get install` step covering: `build-essential cmake git pkg-config libogre-14.2-dev libbullet-dev libsdl2-dev libxerces-c-dev libfreeimage-dev libfreetype6-dev libzzip-dev zlib1g-dev xvfb mesa-utils ffmpeg libavcodec-dev libavformat-dev libswscale-dev libxrandr-dev libxinerama-dev libxi-dev libxcursor-dev` (note: Ogre 14 is built from `ext/ogre-source` submodule because jammy has no `libogre-14.x-dev`; Boost program_options retained — `find_package(Boost)` is still required in `cmake/IncludePackages.cmake:15`)
- [x] 1.3 Pin or capture exact installed package versions; write to a workflow artifact (`apt-versions.txt`)
- [x] 1.4 Set `LIBGL_ALWAYS_SOFTWARE=1` at job level for any step that initializes OpenGL

## 2. Build Verification (CI)

- [x] 2.1 Add `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release` step; fail the job on error
- [x] 2.2 Add `cmake --build build -j` step; capture log to `build.log` artifact
- [x] 2.3 Add a smoke step: `./build/bin/yars --iterations 100 --nogui --xml xml/braitenberg_nocontroller.xml`
- [ ] 2.4 If the build fails on Linux, open a follow-up `fix-linux-build-<reason>` proposal; do not patch inline unless trivial

## 3. Headless Audit Corpus (CI)

- [x] 3.1 Add a matrix or loop running each standalone XML config from the `simulation-audit` scope for 500 iterations with `--nogui`
- [x] 3.2 Add a matrix or loop running each controller-based config for 500 iterations with `--nogui --lib <contrib_lib>`
- [x] 3.3 Upload per-config stdout/stderr under `linux-audit-logs/` as a workflow artifact
- [x] 3.4 Run `braitenberg_logging.xml` and diff its CSV output against `xml/reference_logfile.csv`; fail the job on diff (or document a known cross-platform numerical variance) — currently warning-only until first-run baseline is captured

## 4. FFmpeg Video Capture (CI)

- [x] 4.1 Wrap the GL-initializing capture step with `xvfb-run -s "-screen 0 1920x1080x24"` so Ogre 14 can create an X drawable under llvmpipe
- [x] 4.2 Run a frame-capture configuration (`xml/test_capture.xml` or equivalent) for ~10 seconds of simulation
- [x] 4.3 Run the MP4 assembly step and confirm a playable `.mp4` is produced
- [x] 4.4 Verify the MP4 with `ffprobe`; capture metadata to a workflow artifact
- [x] 4.5 Upload the MP4 itself as a workflow artifact for visual inspection

## 5. X11 GUI Verification (VMware Fusion 13, local)

- [ ] 5.1 Install Ubuntu Server 22.04.2+ (NOT 22.04 or 22.04.1 — those ISOs fail on Apple Silicon Fusion) into a Fusion 13 VM (4 GB RAM, 2 cores, vTPM enabled)
- [ ] 5.2 Install HWE kernel: `sudo apt install linux-generic-hwe-22.04` and reboot
- [ ] 5.3 Install Mesa ≥ 24.0.5 (from kisak PPA if jammy stock is older): `sudo add-apt-repository ppa:kisak/kisak-mesa && sudo apt update && sudo apt full-upgrade`
- [ ] 5.4 Precondition check: `glxinfo | grep "OpenGL core profile version"` MUST report ≥ 3.3 with a non-llvmpipe renderer; if not, document the failure and stop — do not run YARS yet
- [ ] 5.5 Clone YARS, install dev deps (same set as CI), build with `cmake --build build -j`
- [ ] 5.6 Launch `./build/yars --xml xml/braitenberg_logging.xml --lib <contrib_lib>`; confirm a window opens and remains stable for 30 seconds
- [ ] 5.7 Confirm robot geometry, ground plane, and ground texture render correctly (compare visually with the macOS reference video)
- [ ] 5.8 Exercise camera input (mouse drag, scroll zoom); confirm responses are smooth
- [ ] 5.9 Capture a screenshot to `docs/planning/linux-screenshots/braitenberg.png`

## 6. Documentation and Acceptance

- [ ] 6.1 Write `docs/planning/linux-port-verification-status.md` containing: CI runner image SHA, `apt-versions.txt`, build log excerpt, per-config audit results, CSV-diff result, MP4 metadata, Fusion VM specs (distro, kernel, Mesa version, glxinfo output), and GUI screenshot link
- [ ] 6.2 Refresh `docs/Linux_Build.md` with the CI-validated `apt-get` line and an explicit "llvmpipe fallback" section
- [ ] 6.3 Acceptance: CI workflow passes on a feature branch (build + headless audit + MP4 capture all green)
- [ ] 6.4 Acceptance: Fusion GUI session opens, renders, accepts camera input, screenshot checked in
- [ ] 6.5 Acceptance: FFmpeg path produces a playable MP4 verifiable with `ffprobe`
- [ ] 6.6 Any defects surfaced by the audit are filed as separate OpenSpec change proposals, not patched inline
