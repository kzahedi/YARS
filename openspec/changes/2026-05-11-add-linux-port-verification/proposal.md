## Why

CMake and build documentation were updated to support Linux in February 2026, but the port has never been exercised on an actual Linux system. The Linux-Port plan listed three open validation items — tested on Linux, X11 window integration verified, and video capture tested. Without a Linux verification pass, the build files are speculative and may bit-rot. This change establishes a verified Linux baseline so the port can be declared supported (or specific gaps can be filed as fix proposals).

## What Changes

- **Provision a Linux environment**: Either a UTM/Ubuntu 22.04 LTS VM (per `Linux_Port.md`) or a CI runner — whichever is faster to bring up
- **Document dependencies**: Capture the exact `apt-get install` lines for Ogre 14, Bullet, SDL2, Xerces-C++, and optional FFmpeg on Ubuntu 22.04
- **Verify build from scratch**: Clone YARS, configure, and build on the Linux environment; capture any required CMake or source changes
- **Run the simulation-audit corpus headless on Linux**: Re-run the audit corpus with `--nogui` on Linux to confirm physics works
- **Verify X11 GUI**: Run a representative GUI config (`braitenberg_logging.xml`) and confirm a window opens, shapes render, and camera input is received
- **Verify FFmpeg video capture on Linux**: Re-run a frame-capture config and confirm an MP4 is produced
- **Document the baseline**: A status doc records the Linux distribution, package versions, build output, and audit results

## Capabilities

### New Capabilities

- `linux-port-verification`: Verified Linux baseline covering build, headless simulation, X11 GUI, and FFmpeg video capture

### Modified Capabilities

<!-- None. simulation-audit and video-export-per-sim are macOS-validated; this capability records a Linux-equivalent run -->

## Impact

- `docs/Linux_Build.md` — refreshed apt-get / cmake instructions if any drift is found
- `cmake/IncludePackages.cmake` — likely no changes; updated only if the verification surfaces a gap
- `docs/planning/linux-port-verification-status.md` — new status doc with package versions, build log excerpt, audit results
- `.github/workflows/linux-build.yml` (optional, deferred) — only if a CI runner is the chosen verification environment
- Source code: no logic changes from the verification itself; any defects ship as separate change proposals
