## Why

CMake and build documentation were updated to support Linux in February 2026, but the port has never been exercised on an actual Linux system. The Linux-Port plan listed three open validation items — tested on Linux, X11 window integration verified, and video capture tested. Without a Linux verification pass, the build files are speculative and may bit-rot. This change establishes a verified Linux baseline so the port can be declared supported (or specific gaps can be filed as fix proposals).

## Verification environment (validated 2026-05-12)

Empirical research before this change started found that hardware-accelerated OpenGL 3.3 core (which Ogre 14's `RenderSystem_GL3Plus` requires) is unreliable in ARM Linux guests on Apple Silicon as of mid-2026:

- **UTM 4.6 + Ubuntu ARM64** — `virtio-gpu-gl` caps Linux guests at GL 2.1 / GLES 3.0; 4.6.4 introduced crash regressions; virgl is disabled-by-default in recent UTM builds. Not usable for this verification.
- **VMware Fusion 13 + Ubuntu ARM64** — Works only with strict version discipline: Ubuntu Server 22.04.2+ installer, HWE kernel ≥ 5.19, Mesa ≥ 24.0.5. Otherwise silently falls back to software rendering.
- **Mesa llvmpipe (software OpenGL)** — Supports GL 4.5 core; Ogre 14 GL3Plus initializes correctly. Reliable across all environments, including CI runners under Xvfb.
- **GitHub Actions `ubuntu-22.04` runner + Xvfb + llvmpipe** — Reliable and reproducible; closes everything except the interactive GUI smoke-test.

Given that, this change uses two environments rather than one:

| Environment | Closes | Why |
|---|---|---|
| GitHub Actions `ubuntu-22.04` | Build, headless audit corpus, FFmpeg video capture | CPU-only, reproducible, no local provisioning |
| VMware Fusion 13 + Ubuntu Server 22.04.2 HWE | Interactive X11 GUI verification only | Only path that can show a real desktop session on this host |

UTM is explicitly out of scope and not used. `LIBGL_ALWAYS_SOFTWARE=1` is documented as the standard fallback for any Linux environment whose GPU driver does not expose GL 3.3 core.

## What Changes

- **Add a GitHub Actions workflow** (`.github/workflows/linux-build.yml`) that, on `ubuntu-22.04`:
  - Installs Ogre 14, Bullet, SDL2, Xerces-C++, FFmpeg dev packages
  - Builds YARS from scratch
  - Runs the `simulation-audit` corpus headless under Xvfb with `LIBGL_ALWAYS_SOFTWARE=1`
  - Runs the FFmpeg frame-capture path and uploads the produced MP4 as a build artifact
  - Diffs `braitenberg_logging.xml` CSV output against `xml/reference_logfile.csv`
- **Document VMware Fusion 13 GUI verification path** with explicit version discipline (Ubuntu Server 22.04.2+, HWE kernel, Mesa ≥ 24.0.5) and a `glxinfo` precondition check.
- **Run the X11 GUI verification** in Fusion: launch `braitenberg_logging.xml`, confirm window stability for 30 seconds, capture a screenshot, exercise mouse drag / scroll-zoom.
- **Document the baseline** in `docs/planning/linux-port-verification-status.md` (distribution, package versions, build excerpt, CI run link, audit results, GUI screenshot, MP4 metadata).
- **Refresh `docs/Linux_Build.md`** with the CI-validated `apt-get` set and the llvmpipe fallback note.

## Capabilities

### New Capabilities

- `linux-port-verification`: Verified Linux baseline covering build, headless simulation, X11 GUI, and FFmpeg video capture.

### Modified Capabilities

<!-- None. simulation-audit and video-export-per-sim are macOS-validated; this capability records a Linux-equivalent run. -->

## Impact

- `.github/workflows/linux-build.yml` — new CI workflow (primary deliverable for headless verification)
- `docs/Linux_Build.md` — refreshed apt-get / cmake / llvmpipe instructions
- `cmake/IncludePackages.cmake` — only changed if CI surfaces a gap
- `docs/planning/linux-port-verification-status.md` — new status doc
- `docs/planning/linux-screenshots/braitenberg.png` — new GUI screenshot from Fusion run
- Source code: no logic changes from the verification itself; any defects ship as separate change proposals
