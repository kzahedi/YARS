# Building YARS

This guide provides detailed instructions for building YARS on supported platforms.

## Requirements

### Compiler

- **C++17** compatible compiler
  - macOS: Xcode Command Line Tools (clang 12+)
  - Linux: GCC 9+ or Clang 10+ (planned)

### Dependencies

| Dependency | Minimum Version | Notes |
|------------|-----------------|-------|
| CMake | 3.16 | Build system generator |
| CLI11 | 2.3 | Command-line parsing (header-only) |
| Xerces-C | 3.2 | XML parsing |
| Bullet Physics | 3.0 | Physics simulation |
| OGRE | 14.0 | 3D rendering (built as submodule) |
| SDL2 | 2.0 | Window and input handling |
| Catch2 | 3.0 | Unit testing framework (optional) |

### Optional Dependencies

| Dependency | Purpose |
|------------|---------|
| FFmpeg | Video capture support |
| Qt5 | GUI tools |
| Doxygen | API documentation generation |

## macOS Build

### 1. Install Dependencies

Using Homebrew:

```bash
brew install cmake cli11 xerces-c bullet sdl2
```

Optional (for tests and extras):
```bash
brew install catch2 ffmpeg qt@5 doxygen
```

### 2. Clone Repository

```bash
git clone --recursive https://github.com/kzahedi/YARS.git
cd YARS
```

If you already cloned without `--recursive`:
```bash
git submodule update --init --recursive
```

### 3. Build OGRE (Submodule)

OGRE 14 is included as a Git submodule and built as frameworks for macOS:

```bash
cd ext/ogre
mkdir build && cd build
cmake .. \
    -DOGRE_BUILD_COMPONENT_BITES=OFF \
    -DOGRE_BUILD_SAMPLES=OFF \
    -DOGRE_BUILD_TOOLS=OFF \
    -DOGRE_INSTALL_DOCS=OFF \
    -DOGRE_BUILD_PLUGIN_STBI=ON \
    -DOGRE_BUILD_RENDERSYSTEM_GL3PLUS=ON \
    -DOGRE_BUILD_RENDERSYSTEM_GL=OFF \
    -DOGRE_BUILD_RENDERSYSTEM_GLES2=OFF \
    -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)
cd ../../..
```

### 4. Build YARS

```bash
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

### 5. Run

```bash
./bin/yars ../xml/braitenberg.xml
```

## CMake Options

Configure build options with `-D<OPTION>=<VALUE>`:

| Option | Default | Description |
|--------|---------|-------------|
| `YARS_USE_VISUALISATION` | ON | Enable 3D visualization |
| `YARS_USE_CAPTURE_VIDEO` | OFF | Enable FFmpeg video recording |
| `YARS_BUILD_TESTS` | OFF | Build Catch2 unit and integration tests |
| `YARS_TESTCASES` | OFF | Build legacy test suite |
| `YARS_USE_SOFT_BODIES` | OFF | Enable soft body physics |
| `YARS_BUILD_CPP_CLIENT` | OFF | Build C++ client libraries |
| `YARS_BUILD_JAVA_CLIENT` | OFF | Build Java bindings |
| `YARS_DOXYGEN_DOCUMENTATION` | OFF | Generate API docs |

Example with options:
```bash
cmake .. -DYARS_USE_CAPTURE_VIDEO=ON -DYARS_BUILD_TESTS=ON
```

## Build Configurations

### Release Build (Default)

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)
```

### Debug Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(sysctl -n hw.ncpu)
```

## Running Tests

Build and run the Catch2 test suite:

```bash
cmake .. -DYARS_BUILD_TESTS=ON
make -j$(sysctl -n hw.ncpu)
ctest --output-on-failure
```

Or run tests directly:
```bash
./bin/yars_unit_tests
./bin/yars_integration_tests
```

Test coverage includes:
- **Unit tests**: P3D, Quaternion, Matrix, CircularBuffer, StringTokeniser, Colour
- **Integration tests**: Configuration loading, simulation execution

## Generated Configuration Files

The build process generates several configuration files in the build directory:

| File | Purpose |
|------|---------|
| `plugins.cfg` | OGRE plugin configuration |
| `ogre.cfg` | OGRE render system settings |
| `resources.cfg` | Resource locations for materials, meshes, etc. |
| `rtss.cfg` | Runtime Shader System configuration |

These files use absolute paths and are configured by CMake.

## Troubleshooting

### OGRE Framework Not Found

Ensure OGRE is built in `ext/ogre/build` with frameworks enabled:
```bash
ls ext/ogre/build/lib/*.framework
```

Should show: `Ogre.framework`, `OgreOverlay.framework`, `OgreRTShaderSystem.framework`

### CLI11 Not Found

Install CLI11 via Homebrew:
```bash
brew install cli11
```

### SDL2 Linking Issues

Ensure SDL2 is installed via Homebrew:
```bash
brew reinstall sdl2
```

### Shader Compilation Errors

YARS requires OpenGL 3.3+ (GL3Plus). Verify your GPU supports this:
```bash
system_profiler SPDisplaysDataType | grep -i opengl
```

## Development Setup

### IDE Support

**Xcode:**
```bash
cmake .. -G Xcode
open yars.xcodeproj
```

**VS Code:**
Install the CMake Tools extension and open the YARS folder.

### Code Formatting

The project uses standard C++ formatting. Consider using clang-format with the provided style.

## Linux Build

### 1. Install Dependencies

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install cmake g++ libxerces-c-dev libbullet-dev libsdl2-dev \
                 libgl1-mesa-dev libx11-dev libxt-dev libfreetype6-dev \
                 libfreeimage-dev zlib1g-dev
```

For tests:
```bash
sudo apt install catch2
```

For CLI11 (if not available via apt):
```bash
# CLI11 is header-only, can be installed manually
git clone https://github.com/CLIUtils/CLI11.git
cd CLI11 && mkdir build && cd build
cmake .. -DCLI11_BUILD_TESTS=OFF -DCLI11_BUILD_EXAMPLES=OFF
sudo make install
```

### 2. Build OGRE 14 (Submodule)

```bash
cd ext/ogre
mkdir build && cd build
cmake .. \
    -DOGRE_BUILD_COMPONENT_BITES=ON \
    -DOGRE_BUILD_SAMPLES=OFF \
    -DOGRE_BUILD_TOOLS=OFF \
    -DOGRE_INSTALL_DOCS=OFF \
    -DOGRE_BUILD_PLUGIN_STBI=ON \
    -DOGRE_BUILD_RENDERSYSTEM_GL3PLUS=ON \
    -DOGRE_BUILD_RENDERSYSTEM_GL=OFF \
    -DOGRE_BUILD_RENDERSYSTEM_GLES2=OFF \
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

### 4. Set Library Path

```bash
export LD_LIBRARY_PATH=$PWD/../ext/ogre/build/lib:$LD_LIBRARY_PATH
./bin/yars ../xml/braitenberg.xml
```

### Linux Troubleshooting

**X11 display errors:**
```bash
export DISPLAY=:0
```

**Missing GL headers:**
```bash
sudo apt install libgl1-mesa-dev libglu1-mesa-dev
```

**OGRE plugin loading failures:**
Ensure `plugins.cfg` points to the correct plugin paths in `ext/ogre/build/lib/OGRE/`.

## Installation

```bash
sudo make install
```

Default installation prefix: `/usr/local`

To change:
```bash
cmake .. -DCMAKE_INSTALL_PREFIX=/your/path
```

## Uninstallation

```bash
sudo make uninstall
```
