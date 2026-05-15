# Building YARS on macOS

> Status: verified on macOS 14 (Sonoma) Apple Silicon. Earlier Intel
> macOS versions are likely fine too — Homebrew package names and the
> Ogre build flags are the same.

YARS targets **OpenGL 3.3+ core profile** via the Ogre3D 14 GL3+
renderer. Ogre is built from the bundled submodule rather than a
Homebrew formula (there isn't one).

## 1. Prerequisites

### Homebrew packages

```bash
brew install \
  cmake pkg-config ninja \
  sdl2 bullet xerces-c boost \
  freeimage freetype \
  ffmpeg catch2
```

> Note: `zziplib` was removed from Homebrew core. Ogre treats ZZip as
> optional; the build is configured to skip the ZIP archive codec.

### Clone with submodules

```bash
git clone --recursive https://github.com/kzahedi/YARS.git
cd YARS
# or, if you already cloned without --recursive:
git submodule update --init --recursive
```

The `ext/ogre-source` submodule is large (~135 MB checked out).

## 2. Build Ogre 14

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
  -DOGRE_BUILD_PLUGIN_DOT_SCENE=OFF \
  -DOGRE_BUILD_RENDERSYSTEM_GL3PLUS=ON \
  -DOGRE_BUILD_RENDERSYSTEM_GL=OFF \
  -DOGRE_BUILD_RENDERSYSTEM_METAL=OFF \
  -DOGRE_INSTALL_PDB=OFF \
  -DOGRE_INSTALL_SAMPLES=OFF \
  -DOGRE_BUILD_DEPENDENCIES=OFF

cmake --build ext/ogre/build -j 4
cmake --install ext/ogre/build
```

The build takes 15–25 minutes on a 4-core machine.

### Install layout

On macOS, Ogre 14 dynamic libraries install as **frameworks** under
`ext/ogre/install/lib/macosx/Release/`:

```
ext/ogre/install/
├── CMake/OgreTargets.cmake              # find_package config
├── include/OGRE/                         # public headers
├── lib/macosx/Release/
│   ├── Ogre.framework
│   ├── OgreOverlay.framework
│   ├── OgreRTShaderSystem.framework
│   └── ...
└── share/OGRE-14.4/Media/                # RTShaderLib + shaders
```

YARS's CMake uses `find_package(OGRE CONFIG)` to consume the
exported `OgreTargets.cmake`, so the framework layout is handled
transparently — you don't need to point at individual `.dylib` files.

### Why `OGRE_BUILD_PLUGIN_DOT_SCENE=OFF`

`Plugin_DotScene` requires `pugixml`, which isn't a Homebrew package
YARS depends on. The DotScene format isn't used by YARS, so the
plugin is disabled.

### Why `OGRE_BUILD_RENDERSYSTEM_METAL=OFF`

YARS uses GL3+ on macOS for code-sharing with Linux. Metal would be
nominally faster on Apple Silicon but the YARS rendering pipeline
isn't ready for it.

## 3. Build YARS

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 4
```

Resulting binary: `build/bin/yars`. Controller libraries are at
`build/lib/libYarsController*.dylib`.

## 4. Running

YARS resolves controller libraries from `cwd/lib/` by default, so
launch from the `build/` directory:

```bash
cd build
./bin/yars --xml ../xml/braitenberg.xml
```

Common flags:

| Flag | Meaning |
|---|---|
| `--nogui` | Headless mode. Required for batch / CI. |
| `--iterations N` | Cap simulation steps. |
| `--xml path/to/scene.xml` | Scene configuration. |
| `--lib <path>` | Look for controllers under `<path>/lib` or `<path>`. Falls back to `cwd/lib`, `~/.yars/lib`, `/usr/local/lib`. |
| `--frames` | Enable per-frame PNG capture. Off by default. |

## 5. CI reference

The authoritative recipe lives in `.github/workflows/macos-build.yml`.
If something in this document drifts from CI, CI wins — the workflow
is what's actually exercised on every push.

## 6. Refreshing a stale Ogre install

If you previously built Ogre 13.x (e.g. before the macOS migration to
Ogre 14), delete the cached install and rebuild:

```bash
rm -rf ext/ogre/install ext/ogre/build
# re-run the cmake invocation from section 2
```

## 7. See also

- `docs/Linux_Build.md` — same instructions for Linux. Most steps are
  identical; the differences are confined to the Homebrew vs apt
  package list and Ogre's install layout.
- `docs/planning/linux-port-verification-status.md` — consolidated
  verification evidence for both platforms.
