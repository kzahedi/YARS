## 1. Environment Provisioning

- [ ] 1.1 Decide: UTM/Ubuntu 22.04 VM vs CI runner; document choice and rationale
- [ ] 1.2 Bring up the chosen environment with at least 4 GB RAM and 40 GB disk
- [ ] 1.3 Confirm graphics support (X11 / Xvfb / hardware GPU if available)
- [ ] 1.4 Install build essentials: `build-essential cmake git pkg-config`

## 2. Dependency Installation

- [ ] 2.1 Install Ogre 14 dev packages (or build from source if no package exists)
- [ ] 2.2 Install Bullet Physics dev: `libbullet-dev`
- [ ] 2.3 Install SDL2 dev: `libsdl2-dev`
- [ ] 2.4 Install Xerces-C++ dev: `libxerces-c-dev`
- [ ] 2.5 Install optional FFmpeg: `ffmpeg libavcodec-dev libavformat-dev libswscale-dev`
- [ ] 2.6 Capture exact package versions to `docs/planning/linux-port-verification-status.md`

## 3. Build Verification

- [ ] 3.1 Clone YARS into the Linux environment
- [ ] 3.2 `cmake -S . -B build` and capture full output
- [ ] 3.3 `cmake --build build -j` and confirm clean build
- [ ] 3.4 If build fails, file a follow-up change `fix-linux-build-<reason>` and patch inline only for trivial issues
- [ ] 3.5 Run baseline sanity: `./build/yars --iterations 100 --nogui --xml xml/braitenberg_nocontroller.xml`

## 4. Headless Simulation Audit on Linux

- [ ] 4.1 Run each standalone config from `simulation-audit` scope for 500 iterations `--nogui`
- [ ] 4.2 Run each controller-based config for 500 iterations `--nogui --lib`
- [ ] 4.3 Capture output to `docs/planning/linux-audit-logs/<config-name>.log`
- [ ] 4.4 Compare CSV output of `braitenberg_logging.xml` against `xml/reference_logfile.csv`

## 5. X11 GUI Verification

- [ ] 5.1 With X11 (or Xvfb) running, launch `./build/yars --xml xml/braitenberg_logging.xml --lib <contrib_lib>`
- [ ] 5.2 Confirm a window opens and remains stable for 30 seconds
- [ ] 5.3 Confirm basic geometry renders (robot bodies, ground plane)
- [ ] 5.4 Confirm camera input (mouse drag, scroll zoom) is received and applied
- [ ] 5.5 Capture a screenshot to `docs/planning/linux-screenshots/braitenberg.png`

## 6. FFmpeg Video Capture on Linux

- [ ] 6.1 Run a frame-capture config (`test_capture.xml` or equivalent) for ~10 seconds
- [ ] 6.2 Confirm frames are emitted to disk
- [ ] 6.3 Run the MP4 assembly step and confirm a playable `.mp4` is produced
- [ ] 6.4 Verify the MP4 with `ffprobe`; capture metadata to the status doc

## 7. Documentation and Acceptance

- [ ] 7.1 Write `docs/planning/linux-port-verification-status.md` with distribution, packages, build output excerpt, audit pass/fail, screenshots, and MP4 metadata
- [ ] 7.2 Refresh `docs/Linux_Build.md` if instructions drifted
- [ ] 7.3 All audit configs pass on Linux (or each failure is documented and ticketed)
- [ ] 7.4 GUI session opens, renders, and accepts camera input
- [ ] 7.5 FFmpeg path produces a playable MP4
