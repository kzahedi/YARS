# Building YARS on Linux

> Status: verified on Ubuntu 22.04 (jammy), x86_64 (GitHub Actions CI) and
> aarch64 (UTM on Apple Silicon).  Other modern Linux distros should work
> with equivalent package names.

YARS targets **OpenGL 3.3+ core profile** via the Ogre3D 14 GL3+ renderer.
Ogre is built from the bundled submodule rather than the distro package;
jammy ships an older Ogre that does not match YARS's expected API.

## 1. Prerequisites

### System packages (Ubuntu 22.04 jammy)

```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential cmake git pkg-config ninja-build \
  libboost-program-options-dev \
  libbullet-dev libsdl2-dev libxerces-c-dev \
  libfreeimage-dev libfreetype6-dev libzzip-dev zlib1g-dev \
  libgl1-mesa-dev libglu1-mesa-dev libx11-dev libxt-dev \
  libxrandr-dev libxinerama-dev libxi-dev libxcursor-dev \
  xvfb mesa-utils mesa-utils-extra \
  ffmpeg libavcodec-dev libavformat-dev libswscale-dev libswresample-dev \
  catch2
```

This is the same list the Linux CI workflow installs
(`.github/workflows/linux-build.yml`).

### Clone with submodules

```bash
git clone --recursive https://github.com/kzahedi/YARS.git
cd YARS
# or, if you already cloned without --recursive:
git submodule update --init --recursive
```

The `ext/ogre-source` submodule is large (~135 MB checked out).  The
first init takes a few minutes.

## 2. Build Ogre 14

YARS expects a local Ogre install under `ext/ogre/install/`.  Build it
once:

```bash
cmake -S ext/ogre-source -B ext/ogre/build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX=$(pwd)/ext/ogre/install \
  -DOGRE_BUILD_COMPONENT_BITES=ON \
  -DOGRE_BUILD_COMPONENT_OVERLAY=ON \
  -DOGRE_BUILD_COMPONENT_RTSHADERSYSTEM=ON \
  -DOGRE_BUILD_SAMPLES=OFF \
  -DOGRE_BUILD_TOOLS=OFF \
  -DOGRE_BUILD_PLUGIN_STBI=ON \
  -DOGRE_BUILD_RENDERSYSTEM_GL3PLUS=ON \
  -DOGRE_BUILD_RENDERSYSTEM_GL=OFF \
  -DOGRE_GLSUPPORT_USE_EGL=OFF \
  -DOGRE_BUILD_DEPENDENCIES=OFF

cmake --build ext/ogre/build -j 4
cmake --install ext/ogre/build
```

The build takes 15–25 minutes on a 4-core machine and produces ~250 MB
of install artifacts.

### Why `OGRE_GLSUPPORT_USE_EGL=OFF`

Ogre 14 defaults its GL3+ context creation to EGL.  On hosts where the
GL driver cannot authenticate against a 3D-capable DRM render node
(common in CI under `xvfb`+llvmpipe, and on hypervisors that expose a
2D-only virtio-gpu device), EGL fails with `libEGL warning: DRI2: failed
to authenticate` followed by `Fail to make context current`.  GLX
sidesteps the DRM auth path entirely and works on every Linux desktop
that has an X server (which is all of them, including XWayland on
Wayland desktops).

## 3. Build YARS

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 4
```

Resulting binary: `build/bin/yars`.  Controller libraries are at
`build/lib/libYarsController*.so`.

## 4. Running

YARS resolves controller `.so` files from `cwd/lib/` by default, so
launch from the `build/` directory:

```bash
cd build
./bin/yars --xml ../xml/braitenberg.xml
```

Common flags:

| Flag | Meaning |
|---|---|
| `--nogui` | Headless mode; physics runs without rendering. Required for SSH / batch / CI. |
| `--iterations N` | Cap simulation steps. Useful for CI smoke tests. |
| `--xml path/to/scene.xml` | Scene configuration. |
| `--frames` | Enable per-frame PNG capture. Off by default. |

The `--lib <path>` flag exists in the help but is currently only
validated, not actually injected into the controller search path —
`cwd/lib/` is the canonical resolution. (Tracked as a usability gap.)

## 5. llvmpipe / software rendering fallback

If your host has no GPU acceleration available (CI runners, headless
servers, VMs without virgl), YARS still works under Mesa's `llvmpipe`
software rasterizer.  Performance is significantly lower (expect
~5–15 fps on a Braitenberg scene at 800×800), but:

- The GLX context creation succeeds (`OGRE_GLSUPPORT_USE_EGL=OFF` is
  what makes this possible — EGL fails earlier).
- All RTSS-generated GLSL shaders compile against `GLSL 4.50 core`.
- Materials, textures, and overlays render correctly.

To force llvmpipe explicitly (e.g. inside `xvfb`):

```bash
LIBGL_ALWAYS_SOFTWARE=1 xvfb-run -a -s "-screen 0 1280x720x24" \
  ./bin/yars --xml ../xml/braitenberg.xml
```

`xvfb-run` provides the virtual X display.

## 6. CI reference

The authoritative recipe lives in `.github/workflows/linux-build.yml`.
If something in this document drifts from CI, CI wins — the workflow
is what's actually exercised on every push.
