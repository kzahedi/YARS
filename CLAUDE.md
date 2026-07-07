# YARS - Yet Another Robot Simulator

## Overview
YARS is a mobile robot simulator written in C++ that was originally developed between 2004-2019. It provides physics simulation via Bullet Physics and 3D graphics rendering for robotics research and education.

## Current Status
✅ **Major Modernization Progress Made**

The codebase has been significantly modernized:
- ✅ CMake build system upgraded to 3.16+ with modern policies
- ✅ C++17 standard enforced with modern compiler settings 
- ✅ Boost dependencies largely removed (boost::filesystem → std::filesystem, boost::program_options → modern CMake)
- ✅ Observer pattern removal started (core components like YarsMainControl, RuntimeControl updated)
- ✅ Namespace organization in progress (29 files now use `namespace yars`)
- ✅ Smart pointer migration and nullptr modernization completed
- ✅ Deprecated copy constructor warnings fixed
- ✅ Sign comparison warnings resolved
- ✅ Full build system working with compatibility maintained

**Validation Status**: ✅ All changes validated with braitenberg_logging.xml test showing identical simulation results

## Technical Architecture

### Core Technologies
| Component | Technology | Status |
|-----------|------------|--------|
| Language | C++17 | ✅ Modernized |
| Build System | CMake 3.16+ | ✅ Modernized |
| Physics | Bullet Physics 3.x | ✅ Working |
| Graphics | Ogre3D 14.x | ✅ Integrated (Linux + macOS, dynamic link) |
| Windowing | SDL2 | ✅ Working |
| Configuration | JSON (nlohmann/json) | ✅ Modernized (XML/Xerces removed) |
| Threading | std::thread | ✅ Modernized |
| Filesystem | std::filesystem | ✅ Modernized |
| Memory | Smart pointers | ✅ Modernized |

### Key Components
- **Main Control**: `src/yars/main/YarsMainControl.cpp` - Central simulation controller
- **Physics**: `src/yars/physics/` - Bullet Physics integration
- **Graphics**: `src/yars/view/gui/` - Ogre3D rendering system
- **Configuration**: `src/yars/configuration/` - JSON parsing and data management
- **Logging**: `src/yars/logging/` - CSV/file output system
- **Utilities**: `src/yars/util/` - Helper classes and infrastructure

## Critical Knowledge

### Ogre3D Integration (Successfully Completed)
The project uses Ogre3D 14.x as a git submodule:
- **Location**: `ext/ogre-source` (git submodule, pinned at the master HEAD it was at the time of the Linux port; see commit hash there)
- **Build**: Built locally per platform; install layout is `ext/ogre/install/`. Linux produces dylibs under `lib/`; macOS produces frameworks under `lib/macosx/Release/`.
- **Linking**: Dynamic. `cmake/IncludePackages.cmake` uses Ogre's exported `find_package(OGRE CONFIG)` to get imported targets (`OgreMain`, `OgreOverlay`, `OgreRTShaderSystem`), with a Linux-style manual-path fallback.
- **Plugins**: Loaded at runtime via `plugins.cfg` (generated from `src/cfg/plugins.cfg.in`). `Ogre::Root("plugins.cfg", "ogre.cfg", "")` reads it and `dlopen`s each plugin from `PluginFolder`. No static-plugin install code path remains.
- **GL Context**: Built with `-DOGRE_GLSUPPORT_USE_EGL=OFF` on both platforms. GLX on Linux, NSOpenGLContext on macOS. EGL is broken on virtio-gpu without virgl (UTM, headless CI under xvfb).
- **Status**: ✅ Fully functional with OpenGL 3.3+ core renderer; RTSS emits GLSL 1.50 core syntax (`in`/`out`).

### Observer Pattern (Fully Removed)
The legacy Observer pattern has been replaced with direct method calls:
- **Status**: ✅ Complete. `src/yars/util/Observer.h`, `Observable.h`, and
  `ObservableMessage.h` are deleted from the tree.
- **Direct calls now drive the simulation lifecycle**:
  - `YarsMainControl::run()` calls `_ypc->step()`, `_ylc->step()`,
    `_rtc->step()` directly each loop iteration
  - Quit signalling uses `__YARS_SET_EXIT(true)` polled by the loop
    (from `KeyHandler::exitSimulation`, `SignalHandler::sighandler`,
    `YarsPhysicsModel` when bullet signals quit, and `SdlWindow`
    on close/Escape)
  - Reset signalling uses `__YARS_SET_RESET_SIMULATION` similarly
- **What's gone**: `notifyObservers` calls, `addObserver` wrappers,
  the `_o`/`_observable` static members, the `notify()` Observer-API
  method on every component, and the `__M_*` message-type constants.

### Configuration System (JSON, Xerces removed)
YARS reads only `.json` configuration files as of the JSON Stage 3 harvest
(v0.9.0 onward). The entire XML/Xerces-C++ layer — the XSD schema, the XSD
generator, the `createXsd` chain on every `Data*` class, and the Xerces SAX
parser — has been deleted:
- **Parsing**: `src/yars/configuration/json/JsonParser.{h,cpp}` replays the
  always-arrays JSON shape (produced historically by the now-deleted
  `XmlToJson` converter) into the same `Data*` object model the SAX parser
  used to build. `DataParseElement` is the shared adapter both parsers fed
  into and is unchanged.
- **Factory Pattern**: 80+ factory classes for object creation (unchanged).
- **Attribute parsing (binding-table pilot, JSON Stage 4)**: the
  attribute-parsing half of `Data*::add(DataParseElement*)` — a
  hand-written block repeated across ~90 classes — has a replacement
  primitive: `yars::AttributeBinding` / `yars::applyAttributes`
  (`src/yars/configuration/data/DataBinding.h/.cpp`). Piloted on the
  shape family (`DataBox`, `DataSphere`, `DataCylinder`, `DataCapsule`,
  `DataPly`); child-element dispatch (pose/dimension/physics/texture/mesh)
  stays hand-written — tables don't model that state machine. Remaining
  families (sensors, actuators, logging, robot/environment roots) are
  each an independent follow-up task; see
  `docs/planning/binding-table-recipe.md` for the mechanical steps,
  including the suppression-retirement step for `leak:Data*` entries in
  `scripts/sanitizer-suppressions.txt`.
- **XML is retired**: non-`.json` input (including the old `-` stdin mode)
  now exits with a clear error. To convert a legacy `.xml` config, use the
  `--convert` option of the `v0.9.0-last-xml` release tag (the last release
  with `--export` and the Xerces-based XML path); it writes `<file>.json`
  next to the input.
- **Status**: ✅ Complete. `otool -L build/bin/yars | grep -i xerces` is
  empty; no source file under `src/yars` or `tests` references
  `xercesc`/`XMLString`/`createXsd`.

### Memory Management
Now uses modern C++ patterns:
- **Smart Pointers**: Raw pointers replaced with `std::unique_ptr`/`std::shared_ptr`
- **RAII**: Resource management through constructors/destructors
- **Validation**: Braitenberg test shows no functionality regression

## Build Instructions

### Prerequisites
- CMake 3.16+
- C++17 compatible compiler
- Required libraries: SDL2, FreeImage, FreeType, ZZip, zlib
- **Bullet Physics 3.25**: built from source via the `ext/bullet-source` git
  submodule (https://github.com/bulletphysics/bullet3.git @ tag 3.25), not
  from Homebrew/apt. Measured +11.4% headless-physics throughput over the
  Homebrew bottle (see `docs/planning/bullet-simd-benchmark.md`) — the win likely
  comes from omitting Homebrew's `-DBT_USE_EGL=ON -DBULLET2_MULTITHREADING=ON
  -DBUILD_PYBULLET=ON` extras, not from SIMD/NEON (which bullet3 auto-enables
  on Apple arm64 regardless of who builds it). Build into
  `ext/bullet/install/` per-platform, same pattern as `ext/ogre/install/`:
  ```bash
  cmake -S ext/bullet-source -B ext/bullet/build -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
    -DCMAKE_INSTALL_PREFIX=$(pwd)/ext/bullet/install -DBUILD_SHARED_LIBS=ON \
    -DBUILD_BULLET2_DEMOS=OFF -DBUILD_UNIT_TESTS=OFF \
    -DBUILD_CPU_DEMOS=OFF -DBUILD_OPENGL3_DEMOS=OFF \
    -DBUILD_EXTRAS=OFF -DBUILD_PYBULLET=OFF \
    -DUSE_DOUBLE_PRECISION=OFF -DCMAKE_CXX_FLAGS="-O3"
  cmake --build ext/bullet/build -j 8 && cmake --install ext/bullet/build
  ```
  `cmake/IncludePackages.cmake` prefers `ext/bullet/install` when present and
  falls back to the system package (`find_package(Bullet)`) otherwise.

### Build Process
```bash
# Standard build
mkdir build && cd build
cmake ..
make -j4

# Test the build
./bin/yars --iterations 1000 --xml ../xml/braitenberg_logging.json
```

### Validation Commands
```bash
# Basic simulation test
./bin/yars --iterations 1000 --xml xml/braitenberg_logging.json

# Compare with reference logfile
diff braitenberg-*.csv ../xml/reference_logfile.macos-arm64.csv

# Performance test  
time ./bin/yars --iterations 10000 --xml xml/falling_objects.json

# Memory validation
valgrind --leak-check=full ./bin/yars --iterations 100 --xml xml/braitenberg.json
```

## Modernization Achievements

### Build System Modernization ✅
- **CMake**: Upgraded from 3.5 to 3.16+ with modern policies
- **C++17**: Standard enforced with `CMAKE_CXX_STANDARD_REQUIRED ON`
- **Target-based**: Modern CMake practices with proper target linking
- **Dependencies**: Proper Boost, Bullet, and Ogre integration

### Code Modernization ✅  
- **Nullptr**: All NULL replaced with nullptr
- **Smart Pointers**: Memory management modernized
- **Namespacing**: Systematic `namespace yars {}` organization in progress
- **Compiler Warnings**: Fixed deprecated constructors and sign comparison warnings
- **Observer Pattern**: Core components refactored to direct method calls

### Performance & Reliability ✅
- **Validation**: All changes tested with braitenberg simulation
- **Compatibility**: 100% functional equivalence maintained
- **Build Speed**: Improved compilation times
- **Memory Safety**: Smart pointers eliminate manual memory management

## Known Working Features

### Simulation Capabilities
- **Robot Models**: Braitenberg vehicles, hexapods, custom robots
- **Physics**: Accurate collision detection, rigid body dynamics
- **Sensors**: Proximity, light, contact, position, velocity sensors
- **Actuators**: Hinges, sliders, motors, muscle actuators  
- **Environment**: Static and dynamic objects, lighting, textures

### Rendering System
- **3D Graphics**: Ogre3D with OpenGL 3.3 core renderer
- **Materials**: Shader-based rendering with texture support
- **Camera**: Multiple viewpoints, follow modes, auto-capture
- **Overlays**: Text rendering for debugging and information display

### Configuration System
- **JSON Format**: Comprehensive robot and environment definitions
- **Examples**: Working configurations in `xml/` directory (`.json`; the
  directory name is historical)

## Development Guidelines

### Coding Standards
- **C++17**: Use modern language features (auto, range-based for, smart pointers)
- **Namespacing**: All new code should use `namespace yars {}`
- **Memory**: Prefer smart pointers over raw pointers
- **Naming**: Follow existing camelCase convention
- **Headers**: Include guards and minimal dependencies

### Testing Protocol
- **Regression**: Always test with `braitenberg_logging.json`
- **Validation**: Compare logfile output with reference
- **Build**: Clean build required after significant changes
- **Memory**: Run valgrind for memory leak detection

### File Organization
```
src/yars/
├── main/           # Core simulation control
├── physics/        # Bullet Physics integration  
├── view/           # Graphics and visualization
├── configuration/  # JSON parsing and data
├── logging/        # Output and data recording
├── types/          # Data structures and math
└── util/           # Utilities and helpers
```

## Future Modernization Priorities

### Phase 1: Complete Observer Pattern Removal
- Remove remaining ~141 Observer/Observable references
- Simplify control flow with direct method calls
- Remove ObservableMessage infrastructure

### Phase 2: Configuration System Simplification
- JSON migration complete (Xerces/XSD/SAX layer removed, JSON Stage 3)
- Reduce 80+ factory classes to template-based approach
- Improve error messages and validation

### Phase 3: Further C++ Modernization
- Move to C++20 when appropriate
- Consider modules for faster compilation

### Phase 4: Testing Infrastructure
- Add unit tests with Google Test
- Create integration test suite
- Set up continuous integration

## Lessons Learned

### Successful Patterns
- **Incremental Changes**: Small, testable changes prevent regression
- **Validation-Driven**: Always test against working baseline
- **Modern CMake**: Target-based linking is much cleaner
- **Smart Pointers**: Eliminate entire classes of memory bugs

### Technical Insights
- **Ogre3D**: Static linking approach solved external dependency issues
- **Observer Pattern**: Direct calls are faster and easier to debug
- **Boost Replacement**: std::filesystem transition was straightforward
- **Namespace Organization**: Systematic approach prevents conflicts

### Build System Best Practices
- **CMake Policies**: Modern policies avoid deprecated warnings
- **Target Properties**: Cleaner than global variables
- **Git Submodules**: Effective for complex dependencies like Ogre3D
- **Validation**: Always test after each modernization step

## Command Reference

### Build Commands
```bash
# Clean rebuild
make clean && cmake .. && make -j4

# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build  
cmake -DCMAKE_BUILD_TYPE=Release ..
```

### Test Commands
```bash
# Basic functionality
./bin/yars --iterations 1000 --xml xml/braitenberg_logging.json

# No GUI mode
./bin/yars --iterations 1000 --xml xml/braitenberg_logging.json --no-gui

# Performance test
time ./bin/yars --iterations 10000 --xml xml/falling_objects.json
```

### Analysis Commands
```bash
# Check namespace usage
grep -r "namespace yars" src/ --include="*.h" --include="*.cpp" | wc -l

# Check remaining Observer pattern
grep -r "Observable\|Observer" src/ --include="*.h" --include="*.cpp" | grep -v backup | wc -l

# Check Boost dependencies  
ldd ./bin/yars | grep boost
```

This knowledge base captures the current state and provides the foundation for continued modernization efforts.
- NEVER SAY THAT YOU ARE SUCCESSFUL IF YARS DOES NOT COMPILE OR RUN WITHOUT ERROR
- DONT START EVERY REPLY WITH "You are absolutle right" I HATE THAT
- use the image export of yars to check if the simulation is rendering the textures correctly
- make sure to set a timeout when you call yars, in case it does not end
- ALWAYS COMPILE AND RUN yard  IN THE SUBDIRECTORY ./build