# YARS Modernization Task

## Project

- **Repository:** https://github.com/kzahedi/YARS
- **Local Path:** `~/code/YARS`
- **Type:** Mobile robot simulator
- **Original Development:** 2004-2019
- **Dependencies:** Bullet Physics, OGRE 14, SDL2, Xerces-C++, FFmpeg (optional)

## Background

YARS (Yet Another Robot Simulator) was developed over 15 years for robotics research. The codebase uses several external libraries and accumulated technical debt over time.

## Goals

1. **Modern Platform Support** - Make it compile and run on modern macOS/Linux
2. **Modern C++ Standards** - Refactor to C++17 with smart pointers, std::filesystem, etc.
3. **Observer Pattern Removal** - Simplify the overused Observer/Model patterns that make code flow difficult to understand
4. **Ogre3D Texture Fixes** - Fix the texture rendering issues with modern Ogre3D

## Success Criteria

**The task is complete when we can recreate a video showing:**
- 5-6 Braitenberg robots viewed from the top
- Properly rendered textures (ground, walls, robots)
- Visible sensor visualizations
- Robot movement traces
- Stable physics simulation

**Test Configuration:** `xml/braitenberg_zoo.xml`

## Current State (Feb 4, 2026) - SUCCESS CRITERIA MET ✅

**Video Generated:** `~/code/YARS/build/final_video/yars-0001.mp4`
- braitenberg_zoo.xml with multiple robots
- 438 frames at 30fps (~14.6 seconds)
- H.264 MP4 format (6.9MB)

### Completed
- **OGRE 14 Integration** - Full migration from OGRE 1.x to OGRE 14
  - GL3Plus render system (OpenGL 3.3+)
  - Runtime Shader System (RTSS) for automatic shader generation
  - SDL2 + OGRE 14 window integration for macOS
  - Custom font and sky dome shaders
- **OGRE 14 GUI Rendering Fixed (Feb 2026)**
  - Fixed YarsErrorHandler null pointer crash
  - Added static library path sharing for `--lib` option
  - Fixed RTSS shader generation for textured materials
  - Updated Chain.material for proper RTSS compatibility
  - All scene graph nodes now use MaterialManager::resolveMaterialName
- **Mouse Navigation (Feb 2 2026)** - Complete overhaul
  - FPS-style camera controls (left drag = look, right drag = move, scroll = zoom)
  - Fixed macOS modifier keys (Cmd, Option, Ctrl, Shift)
  - Fixed scroll wheel sensitivity and trackpad precision scrolling
  - Fixed SDL window ID filtering for proper event handling
- **Hexapod Simulation (Feb 2 2026)**
  - Fixed XML to use built-in Hexapod controller
  - Fixed gait pattern (robot now walks forward)
- **C++17 Migration** - Boost COMPLETELY REMOVED ✅
  - `boost::thread` → `std::thread`
  - `boost::function` → `std::function`
  - `boost::filesystem` → `std::filesystem`
  - `boost::date_time` → `std::chrono`
  - `boost::tokenizer`/`BOOST_FOREACH` → C++17 operations
  - `boost::program_options` → **CLI11** (header-only library)
  - `boost::circular_buffer` → **Custom CircularBuffer<T>** template
- **Code Modernization**
  - `NULL` → `nullptr` - **509 occurrences** across all source files
  - `override` specifiers added to all virtual method overrides
  - Range-based for loops replacing all `std::vector<T>::iterator` patterns
  - `string_view`, structured bindings, if-with-initializer
  - Smart pointers for local allocations
  - CMake dependencies cleaned up
- **Build System**
  - CMake 3.16+ with C++17
  - **Boost completely removed** - no Boost dependencies remain
  - Catch2 unit testing infrastructure (60 tests across 6 unit test files)
  - **Linux build support added** (CMake configuration ready)
- **Documentation (Feb 4 2026)**
  - BUILDING.md updated with CLI11, Linux instructions
  - ARCHITECTURE.md updated with testing and dependencies
  - PROFILING.md added with performance analysis

### All Major Features Working ✅

- Physics simulation (`--nogui` mode and GUI) ✅
- GUI rendering with OGRE 14 + SDL2 on macOS ✅
- All test simulations: braitenberg.xml, hexapod.xml, braitenberg_zoo.xml, falling_objects.xml, muscle.xml ✅
- Textured materials render with RTSS-generated shaders ✅
- Unit tests (44 tests, 131 assertions via Catch2) ✅
- Mouse navigation (FPS-style controls on macOS) ✅
- Video capture (H.264/MP4 via FFmpeg) ✅

### Optional Enhancements (Tested Feb 3, 2026)

1. **Shadows** - Stencil shadows enabled, visible in video
2. **Following mode** - `./bin/yars --follow 0` works (orbit camera)
   - Modes: 0=Orbit, 1=Tripod, 2=Offset, 3=CenterOrbit
3. **OSD text rendering** - `./bin/yars --osd` launches with on-screen display
4. **Visual verification** ✅ - User confirmed video looks good

## Next Steps: Linux Port

**Status:** Ready for testing

See [[Linux_Port]] for detailed analysis and testing plan.

### Quick Start (UTM + Ubuntu 22.04)

1. Install UTM on macOS
2. Create Ubuntu 22.04 VM (4GB RAM, 2 cores)
3. Install dependencies:
   ```bash
   sudo apt install cmake g++ libxerces-c-dev libbullet-dev libsdl2-dev \
                    libgl1-mesa-dev libx11-dev libxt-dev
   ```
4. Clone and build YARS
5. Test with `./bin/yars ../xml/braitenberg.xml`

### Testing Phases

1. **Headless build** (`-DYARS_USE_VISUALISATION=OFF`)
2. **Unit tests** (`-DYARS_BUILD_TESTS=ON`)
3. **GUI build** (with visualization)
4. **Video capture** validation

## Related Documents

- [[Linux_Port]] - Linux porting analysis and testing plan
- [[Refactoring_Plan]] - Original multi-phase implementation plan
- [[completed]] - Session history with all changes
- In-repo docs: `CLAUDE.md`, `BUILDING.md`, `ARCHITECTURE.md`, `PROFILING.md`

## Notes

- Original task mentioned "rollback to Dec 2019" - but the modernization work aligns with goals, so we build upon it
- Always validate against `braitenberg_logging.xml` reference output
- Use frame capture for visual validation
