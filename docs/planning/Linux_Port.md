# YARS Linux Port Analysis

## Overview

This document analyzes the changes made to support Linux builds for YARS and outlines the testing plan.

## Changes Made (Feb 4, 2026)

### CMake Configuration (`cmake/IncludePackages.cmake`)

Added Linux OGRE configuration alongside macOS:

```cmake
elseif(UNIX)
  # Linux: OGRE builds as shared libraries
  set(OGRE_LIB_DIR "${OGRE_BUILD_DIR}/lib")

  # Include directories
  set(OGRE_INCLUDE_DIRS
    "${OGRE_BUILD_DIR}/include"
    "${OGRE_BUILD_DIR}/include/OGRE"
    "${OGRE_SOURCE_DIR}/OgreMain/include"
    "${OGRE_SOURCE_DIR}/Components/Overlay/include"
    "${OGRE_SOURCE_DIR}/Components/RTShaderSystem/include"
    "${OGRE_SOURCE_DIR}/Components/Bites/include"
  )

  # Libraries (shared objects)
  set(OGRE_LIBRARIES
    "${OGRE_LIB_DIR}/libOgreMain.so"
    "${OGRE_LIB_DIR}/libOgreOverlay.so"
    "${OGRE_LIB_DIR}/libOgreRTShaderSystem.so"
    "${OGRE_LIB_DIR}/libOgreBites.so"
    GL X11 Xt pthread
  )
endif()
```

### Key Differences: macOS vs Linux

| Aspect | macOS | Linux |
|--------|-------|-------|
| OGRE format | Frameworks (`.framework`) | Shared libraries (`.so`) |
| Window system | Cocoa/NSView | X11 |
| GL context | NSOpenGLContext | GLX |
| Library path | `DYLD_LIBRARY_PATH` | `LD_LIBRARY_PATH` |
| Build location | `ext/ogre/build/lib/macosx/` | `ext/ogre/build/lib/` |

### Platform-Specific Code in Source

**SdlWindow.cpp:560** - X11 window handle:
```cpp
#ifdef __linux__
  params["parentWindowHandle"] = Ogre::StringConverter::toString(
    (unsigned long)syswm_info.info.x11.window);
#endif
```

**OgreHandler.cpp:117** - Linux uses ogre.cfg:
```cpp
#ifdef __APPLE__
  _root = new Ogre::Root("plugins.cfg", "", "");
#else
  _root = new Ogre::Root("plugins.cfg", "ogre.cfg", "");
#endif
```

**Directories.cpp:24** - Path delimiter:
```cpp
#ifdef __linux__
#define YARS_DIR_DELIMITER "/"
#define YARS_INSTALL_SUFFIX "/yars"
#endif
```

## Linux Dependencies

### Ubuntu/Debian Packages

```bash
# Core build tools
sudo apt install cmake g++ build-essential

# YARS dependencies
sudo apt install libxerces-c-dev libbullet-dev libsdl2-dev

# OGRE dependencies
sudo apt install libgl1-mesa-dev libx11-dev libxt-dev \
                 libfreetype6-dev libfreeimage-dev zlib1g-dev

# Optional: testing
sudo apt install catch2

# CLI11 (if not in repos)
git clone https://github.com/CLIUtils/CLI11.git
cd CLI11 && mkdir build && cd build
cmake .. -DCLI11_BUILD_TESTS=OFF && sudo make install
```

## Build Steps

### 1. Clone Repository

```bash
git clone --recursive https://github.com/kzahedi/YARS.git
cd YARS
```

### 2. Build OGRE 14

```bash
cd ext/ogre
mkdir build && cd build
cmake .. \
    -DOGRE_BUILD_COMPONENT_BITES=ON \
    -DOGRE_BUILD_SAMPLES=OFF \
    -DOGRE_BUILD_TOOLS=OFF \
    -DOGRE_BUILD_PLUGIN_STBI=ON \
    -DOGRE_BUILD_RENDERSYSTEM_GL3PLUS=ON \
    -DOGRE_BUILD_RENDERSYSTEM_GL=OFF \
    -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
cd ../../..
```

### 3. Build YARS

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 4. Run

```bash
export LD_LIBRARY_PATH=$PWD/../ext/ogre/build/lib:$LD_LIBRARY_PATH
./bin/yars ../xml/braitenberg.xml
```

## Potential Issues to Watch

### 1. Plugin Loading

Plugins are loaded directly in code (`OgreHandler.cpp`), not via `plugins.cfg`. This should work but may need adjustments if OGRE can't find:
- `libRenderSystem_GL3Plus.so`
- `libPlugin_ParticleFX.so`
- `libCodec_STBI.so`

### 2. X11 Window Integration

The `parentWindowHandle` parameter works differently than macOS's `externalWindowHandle`. May need to experiment with:
```cpp
params["externalGLControl"] = "1";
params["currentGLContext"] = "1";
```

### 3. OpenGL Context

Linux uses GLX, not Cocoa. The `SDL_GL_CreateContext` should work, but GL 3.3+ capability depends on drivers.

### 4. Library RPATH

After build, libraries are in `ext/ogre/build/lib/`. Either:
- Set `LD_LIBRARY_PATH` at runtime
- Or add `-DCMAKE_INSTALL_RPATH` during build

## Testing Plan

### Phase 1: Headless Build

```bash
cmake .. -DYARS_USE_VISUALISATION=OFF
make -j$(nproc)
./bin/yars --iterations 1000 ../xml/braitenberg_logging.xml
```

### Phase 2: Unit Tests

```bash
cmake .. -DYARS_BUILD_TESTS=ON -DYARS_USE_VISUALISATION=OFF
make -j$(nproc)
ctest --output-on-failure
```

### Phase 3: GUI Build

```bash
cmake .. -DYARS_USE_VISUALISATION=ON
make -j$(nproc)
./bin/yars ../xml/braitenberg.xml
```

### Phase 4: Full Validation

```bash
./bin/yars --iterations 500 --capture ../xml/braitenberg_zoo.xml
```

## VM Setup (UTM + Ubuntu)

1. Download UTM: https://mac.getutm.app
2. Create VM: Linux → Ubuntu 22.04 LTS
3. Resources: 4GB RAM, 2+ cores, 40GB disk
4. Install Ubuntu, then run dependency commands above
5. Clone YARS and test

## Status

- [x] CMake Linux configuration added
- [x] Build documentation updated
- [ ] Tested on actual Linux machine
- [ ] X11 window integration verified
- [ ] Video capture tested on Linux

## Related Documents

- [[personal/YARS/Task]] - Main task tracking
- [[completed]] - Session history
- [[Refactoring_Plan]] - Original refactoring roadmap
