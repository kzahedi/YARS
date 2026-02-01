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
| Boost | 1.70 | program_options, filesystem, thread |
| Xerces-C | 3.2 | XML parsing |
| Bullet Physics | 3.0 | Physics simulation |
| OGRE | 14.0 | 3D rendering (built as submodule) |
| SDL2 | 2.0 | Window and input handling |

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
brew install cmake boost xerces-c bullet sdl2
```

Optional:
```bash
brew install ffmpeg qt@5 doxygen
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
| `YARS_TESTCASES` | OFF | Build test suite |
| `YARS_USE_SOFT_BODIES` | OFF | Enable soft body physics |
| `YARS_BUILD_CPP_CLIENT` | OFF | Build C++ client libraries |
| `YARS_BUILD_JAVA_CLIENT` | OFF | Build Java bindings |
| `YARS_DOXYGEN_DOCUMENTATION` | OFF | Generate API docs |

Example with options:
```bash
cmake .. -DYARS_USE_CAPTURE_VIDEO=ON -DYARS_TESTCASES=ON
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

### Missing Boost Components

Install all required Boost libraries:
```bash
brew install boost
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

## Linux Build (Planned)

Linux support is planned but not yet fully implemented. The build system is prepared but OGRE visualization is currently disabled on Linux.

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
