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

## 5. X11 GUI Verification (UTM on Apple Silicon, local)

Note: original plan called for VMware Fusion 13. Fusion installer was corrupted on the verification host; switched to UTM (free, Apple Silicon-native). On macOS 13+, UTM's "Virtualize" mode uses Apple's Virtualization framework, which **does not implement virgl for Linux guests** — the guest virtio-gpu device is 2D-framebuffer only and Mesa correctly falls back to llvmpipe (software). This was confirmed empirically (`[drm] features: -virgl`, `MESA-LOADER: dlopen(.../swrast_dri.so)`).

Accepted consequence: GPU-path verification is **deferred** to a future task on real Linux hardware (or a host with virgl-capable virtualization). Acceptance for this change is reduced to:
- YARS launches an X11 window on a real Linux desktop session (not just headless xvfb under CI)
- Scene renders correctly (geometry, ground, textures) under Mesa llvmpipe with OpenGL ≥ 3.3 core
- Mouse drag + scroll-zoom camera input work
- Process is stable for ≥ 30 seconds

CI continues to cover headless amd64 audit. This local pass covers arm64 GUI launch + render + input under software rasterization.

- [x] 5.1 Install UTM (`brew install --cask utm` or https://mac.getutm.app) — done
- [x] 5.2 Install Ubuntu **arm64** into a UTM "Virtualize" VM (4 GB RAM, 2–4 cores, 30 GB disk) — done; running Ubuntu 22.04 with HWE kernel
- [x] 5.3 Install desktop + GL utilities: `sudo apt install ubuntu-desktop-minimal mesa-utils` — done
- [x] 5.4 Install HWE kernel: `sudo apt install linux-generic-hwe-22.04` and reboot — done
- [ ] ~~5.5 Install Mesa ≥ 24.0.5 from kisak PPA~~ — N/A; Mesa version does not matter once we accept llvmpipe. Skipped.
- [x] 5.6 Renderer precondition relaxed: llvmpipe accepted on UTM/Apple Silicon. Confirmed OpenGL 4.5 core via Mesa 23.2.1; `direct rendering: Yes`. Documented in change rationale above.
- [ ] 5.7 Set up SSH access from host Mac to the VM (port-forward host:2222 → guest:22, key-based auth) so subsequent steps can be run directly from the dev machine
- [ ] 5.8 Clone YARS, install dev deps (same `apt-get` line as CI), build with `cmake --build build -j` — blocked on the Ogre 14 `STBIPlugin` rename fix landing on the branch first
- [ ] 5.9 Launch `./build/bin/yars --xml xml/braitenberg_logging.xml`; confirm a window opens and remains stable for 30 seconds
- [ ] 5.10 Confirm robot geometry, ground plane, and ground texture render correctly (compare visually with the macOS reference video). Expect slow framerate on llvmpipe — acceptable.
- [ ] 5.11 Exercise camera input (mouse drag, scroll zoom); confirm responses are smooth enough to be usable
- [ ] 5.12 Capture a screenshot to `docs/planning/linux-screenshots/braitenberg.png`
- [ ] 5.13 File a follow-up OpenSpec change `add-linux-gpu-path-verification` to track real-hardware GL verification once a virgl-capable host (or physical Linux box) is available

## 6. Documentation and Acceptance

- [ ] 6.1 Write `docs/planning/linux-port-verification-status.md` containing: CI runner image SHA, `apt-versions.txt`, build log excerpt, per-config audit results, CSV-diff result, MP4 metadata, Fusion VM specs (distro, kernel, Mesa version, glxinfo output), and GUI screenshot link
- [ ] 6.2 Refresh `docs/Linux_Build.md` with the CI-validated `apt-get` line and an explicit "llvmpipe fallback" section
- [ ] 6.3 Acceptance: CI workflow passes on a feature branch (build + headless audit + MP4 capture all green)
- [ ] 6.4 Acceptance: Fusion GUI session opens, renders, accepts camera input, screenshot checked in
- [ ] 6.5 Acceptance: FFmpeg path produces a playable MP4 verifiable with `ffprobe`
- [ ] 6.6 Any defects surfaced by the audit are filed as separate OpenSpec change proposals, not patched inline
