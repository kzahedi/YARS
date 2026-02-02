# YARS - Yet Another Robot Simulator

[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/std/the-standard)
[![CMake](https://img.shields.io/badge/CMake-3.16+-green.svg)](https://cmake.org/)

YARS is a physics-based robot simulation platform built with C++17, featuring real-time 3D visualization using OGRE 14 and Bullet Physics for accurate rigid body dynamics.

## Features

- **Physics Simulation**: Powered by Bullet Physics for realistic rigid body dynamics
- **3D Visualization**: OGRE 14 rendering with GL3Plus (OpenGL 3.3+)
- **XML Configuration**: Flexible robot and environment definition via XML/XSD schemas
- **Modular Controllers**: Plugin architecture for robot controllers
- **Multi-platform**: macOS support (Linux planned)
- **Logging System**: CSV, file, console, and Blender export logging

## Demo Videos

- [Creatures in Action](https://www.youtube.com/watch?v=T3gDxaLcmUw)
- [Full Playlist](https://www.youtube.com/playlist?list=PLrIVgT56nVQ55q0DNvVbGHfMf87PBnOM-)

## Quick Start

### Prerequisites

**macOS:**
```bash
brew install cmake boost xerces-c bullet sdl2
```

### Building

```bash
git clone --recursive https://github.com/kzahedi/YARS.git
cd YARS
mkdir build && cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

### Running

```bash
# From build directory
./bin/yars ../xml/braitenberg.xml

# From anywhere (specify lib path for controllers)
./bin/yars --lib /path/to/YARS/build/lib /path/to/YARS/xml/hexapod.xml
```

### Mouse Controls (FPS-style)

| Action | Control |
|--------|---------|
| Look around | Left mouse drag |
| Strafe (move sideways/up/down) | Middle drag or Shift+Left drag |
| Move forward/backward | Right mouse drag (vertical) |
| Move forward/backward | Scroll wheel |

### Command Line Options

```bash
./bin/yars --help                    # Show all options
./bin/yars --lib <path>              # Path to controller libraries
./bin/yars --nogui                   # Run without visualization
./bin/yars --iterations <n>          # Run for n iterations
./bin/yars --realtime                # Real-time mode
```

## Documentation

- [Building Guide](BUILDING.md) - Detailed build instructions
- [Architecture Overview](ARCHITECTURE.md) - System design and components

## Project Structure

```
YARS/
├── src/yars/           # Core source code
│   ├── configuration/  # XML/XSD configuration parsing
│   ├── physics/        # Bullet physics integration
│   ├── view/           # Visualization (OGRE 14 + SDL2)
│   ├── main/           # Application control
│   └── util/           # Utilities and helpers
├── contrib/            # Controllers and client libraries
├── materials/          # OGRE materials and textures
├── shaders/            # GLSL shaders for GL3Plus
├── meshes/             # 3D model files
├── fonts/              # Rendering fonts
├── xml/                # Example simulation configurations
└── cmake/              # CMake build modules
```

## Dependencies

| Library | Version | Purpose |
|---------|---------|---------|
| CMake | 3.16+ | Build system |
| Boost | 1.70+ | Core utilities |
| Xerces-C | 3.x | XML parsing |
| Bullet | 3.x | Physics simulation |
| OGRE | 14.x | 3D rendering |
| SDL2 | 2.x | Window/input handling |

## Controllers

YARS supports multiple controller types:

- **Built-in controllers**: Braitenberg2a/2b/3a/3b, Hexapod, Sine, SquareWave, ConstantValue
- **Python controllers**: Requires Python development headers
- **TCP/IP controllers**: For external controller processes
- **Named Pipe controllers**: For local IPC

### Python Controller Example

```xml
<controller module="Python" frequency="10">
  <parameter name="module" value="hexapod"/>
  <parameter name="working directory" value="../contrib/controller"/>
</controller>
```

## Known Issues

- **macOS**: The message `UNSUPPORTED: gldCopyBufferSubData: NEEDS IMPLEMENTATION` appears but doesn't affect functionality
- OGRE 14 with GL3Plus requires shader-based rendering (no fixed-function pipeline)

## Version History

Current version: **0.7.28**

See [version.h](src/yars/defines/version.h) for detailed change log.

## License

This project is licensed under the BSD 3-Clause License - see the [LICENSE](LICENSE) file for details.

## Third Party Assets

**Fonts:** [fontspace.com](http://www.fontspace.com)

**Images:**
- CBT.jpg - [flickr/stallio](http://www.flickr.com/photos/stallio/3149911976/sizes/o/in/photostream/)
- wall.jpg - [agf81/deviantart](http://agf81.deviantart.com/art/Brick-Seamless-160259435)
- clouds.jpg - [ode.org](http://www.ode.org)

## Author

Keyan Ghazi-Zahedi

## Acknowledgments

- OGRE Team for the excellent 3D rendering engine
- Bullet Physics for the physics simulation library
