# Linux Port Verification — Status

> Snapshot of the verification evidence for the Linux port effort
> tracked by openspec changes `add-linux-port-verification`,
> `fix-linux-build-stbi-and-macros`, `fix-yars-audit-regressions`,
> `migrate-macos-to-ogre-14`.

## Outcome

**YARS builds and runs on Linux** — both headless (CI) and with a real
GUI window (UTM Apple Silicon VM, Ubuntu 22.04 arm64).

## CI evidence

`.github/workflows/linux-build.yml` runs on `ubuntu-22.04` (GitHub
Actions hosted runner image — see workflow run logs for the exact
image SHA) and exercises:

| Step | Status |
|---|---|
| Apt deps install | ✓ |
| Build Ogre 14 from submodule (`OGRE_GLSUPPORT_USE_EGL=OFF`) | ✓ |
| Configure + build YARS | ✓ |
| `--nogui` smoke test (`braitenberg_nocontroller.xml`, 100 iters) | ✓ |
| Headless audit corpus, 500 iterations each: | |
| &nbsp;&nbsp;`braitenberg_nocontroller.xml`, `falling_objects.xml`, `test_capture.xml` | ✓ |
| &nbsp;&nbsp;`braitenberg.xml`, `braitenberg_noise.xml`, `braitenberg_logging.xml` | ✓ |
| &nbsp;&nbsp;`braitenberg_light_source.xml`, `braitenberg_trace_projection.xml`, `braitenberg_zoo.xml` | ✓ |
| &nbsp;&nbsp;`muscle.xml`, `joints/generic_angular.xml`, `joints/generic_force.xml` | ✓ |
| Reference CSV regression (`braitenberg_logging.xml`) | ✓ |
| FFmpeg video capture under xvfb + llvmpipe | ✓ |

The audit step caps each invocation at 60 seconds wall-clock so a hang
in any single config can't burn the 90-minute job budget.

## VM evidence (manual GUI verification)

| Field | Value |
|---|---|
| Host | macOS on Apple Silicon |
| Hypervisor | UTM 4.x (Apple Virtualization framework, "Virtualize" mode) |
| Guest distro | Ubuntu 22.04.5 LTS arm64 |
| Kernel | 5.15+ HWE |
| Mesa | 23.2.1-1ubuntu3.1~22.04.3 |
| GL profile | OpenGL 4.5 core, Mesa Software Rasterizer (`llvmpipe`) |
| `direct rendering` | Yes |
| GUI session | Ubuntu desktop on Xorg (not Wayland) |

The virtio-gpu device exposed by UTM under Apple Virtualization is
**2D-framebuffer only** (dmesg reports `[drm] features: -virgl`).  Mesa
correctly falls back to `swrast_dri.so` (llvmpipe).  YARS runs against
this software path; acceptance for this verification does **not**
include hardware-accelerated GL.  Real-GPU verification is tracked
separately (it requires either physical Linux hardware or a hypervisor
that implements virgl, which Apple Virtualization does not).

## GUI screenshot

`docs/planning/linux-screenshots/braitenberg.png` — `braitenberg.xml`
running with the Braitenberg2b controller, ~12 simulated minutes in,
showing:

- Brick-textured wall and ground-textured floor
- Robot body (green circle) with sensor cone (cream wedges)
- Trace lines for both wheels
- OSD: timestamp, real-time factor, scene name
- Live IR sensor readings ("Left IR sensor: 1.00", "Right IR sensor: 1.00")

## Source-side fixes required

Most of the Linux work was YARS-side code changes, not infrastructure.
Summary, in landing order:

1. **STBI header path** (`OGRE/Plugins/STBICodec/`, not the framework path used on macOS)
2. **Drop `Ogre::STBIPlugin`** — class removed in Ogre 14; codec loaded via plugins.cfg
3. **CMake `CMP0167` policy guard** for cmake < 3.30 (jammy is on 3.22)
4. **Drop `FFPTexturingFactory`** — removed from Ogre 14 public API; RTSS auto-registers
5. **SDL2 via `SDL2::SDL2` imported target** instead of hardcoded `/opt/homebrew/lib/libSDL2.dylib`
6. **Ogre libs from `${OGRE_ROOT}/lib`** instead of hardcoded `/usr/lib/x86_64-linux-gnu`
7. **`RenderSystem_GL3Plus` + `Codec_STBI`** in plugins.cfg (was the legacy `RenderSystem_GL`)
8. **`OgreUnifiedShader.h` IN/OUT macros default to modern GLSL** (was gated on `USE_OGRE_FROM_FUTURE` which was never defined; macros fell into the legacy `attribute decl;` branch)
9. **Ogre rebuilt with `OGRE_GLSUPPORT_USE_EGL=OFF`** to use GLX (EGL fails on virtio-gpu without virgl)
10. **Headless DISPLAY check gated on `--nogui`** in `DataLoggingGnuplot` + `DataLoggingSelforg`
11. **`pthread_mutex_t _mutex` initialised** via `PTHREAD_MUTEX_INITIALIZER` in 17 `Data*` headers (was uninitialised; assertion fired non-deterministically on glibc 2.35)

All fixes preserved macOS behaviour by gating with `#if __APPLE__` or
by being inherently cross-platform (e.g. the in-class mutex
initialiser).

## Open items

- **`migrate-macos-to-ogre-14`** — currently macOS CI pins the Ogre
  submodule to `v13.6.4` because the macOS code paths still target
  Ogre 13 APIs.  Tracked as a separate openspec change.
- **`--lib` arg** is validated but doesn't actually add to the
  controller search path; the cwd/lib pattern works around it.
  Usability gap, not a bug.
- **Hardware-accelerated GL verification** on a non-virtio-gpu host.
- **`docs/Linux_Build.md`** — written; this file is the companion
  verification status doc.
