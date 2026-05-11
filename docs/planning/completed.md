# YARS Modernization - Completed Work

## Session: 2026-02-04

### Boost Completely Removed (COMPLETE)

**Status:** Complete ✅

**Summary:**
Removed ALL remaining Boost dependencies from YARS. The codebase is now Boost-free.

**Changes:**

1. **CLI11 replaces boost::program_options**
   - Header-only C++11 command-line parsing library
   - Rewrote `ProgramOptions.cpp` to use CLI11 API
   - Install: `brew install cli11` (macOS) or build from source (Linux)

2. **Custom CircularBuffer<T> replaces boost::circular_buffer**
   - New file: `src/yars/util/CircularBuffer.h`
   - Template class using `std::deque` internally
   - Drop-in replacement with same API
   - Updated `TraceLineBuffer` to use new implementation

**Files Modified:**
- `src/yars/main/ProgramOptions.cpp` - Complete rewrite for CLI11
- `src/yars/main/ProgramOptions.h` - CLI11 includes, boolean flags
- `src/yars/util/CircularBuffer.h` - New file
- `src/yars/util/TraceLineBuffer.h` - Use CircularBuffer
- `cmake/IncludePackages.cmake` - Remove Boost, add CLI11

**Commits:**
- `refactor: Replace boost::circular_buffer with custom CircularBuffer template`
- `refactor: Replace boost::program_options with CLI11`

---

### Performance Profiling (COMPLETE)

**Status:** Complete ✅

**Summary:**
Profiled YARS using macOS `sample` tool to identify performance bottlenecks.

**Key Finding:** 69% of CPU time spent in Bullet Physics rayTest for proximity sensors.

**Benchmarks:**
| Configuration | Iterations | Time | Rate |
|--------------|-----------|------|------|
| braitenberg.xml (1 robot) | 10,000 | 0.35s | ~28,500 iter/s |
| braitenberg_zoo.xml (5 robots) | 10,000 | 1.55s | ~6,450 iter/s |

**Output:** `~/code/YARS/PROFILING.md`

---

### Expanded Test Coverage (COMPLETE)

**Status:** Complete ✅

**Summary:**
Added more unit tests with Catch2, now covering 6 test files with 60 test cases.

**New Test Files:**
- `tests/unit/test_circular_buffer.cpp` - 10 test cases
- `tests/unit/test_string_tokeniser.cpp` - 6 test cases
- `tests/unit/test_colour.cpp` - 6 test cases

**Total Coverage:**
- P3D, Quaternion, Matrix (existing)
- CircularBuffer, StringTokeniser, Colour (new)

---

### Documentation Update (COMPLETE)

**Status:** Complete ✅

**Files Updated:**
- `BUILDING.md` - Replaced Boost with CLI11, added Linux instructions
- `ARCHITECTURE.md` - Added Testing and Dependencies sections
- `PROFILING.md` - New file with performance analysis

---

### Linux Build Support (COMPLETE)

**Status:** Complete ✅ (Untested - needs Linux machine)

**Summary:**
Added CMake configuration for Linux OGRE visualization.

**Changes:**
- `cmake/IncludePackages.cmake` - Linux OGRE shared library paths
- `BUILDING.md` - Complete Ubuntu/Debian build instructions

**Linux Dependencies:**
```bash
sudo apt install cmake g++ libxerces-c-dev libbullet-dev libsdl2-dev \
                 libgl1-mesa-dev libx11-dev libxt-dev
```

**Next Step:** Test on actual Linux machine (recommend UTM with Ubuntu 22.04)

---

## Session: 2026-02-03

### Video Capture Enabled (SUCCESS CRITERIA MET)

**Status:** Complete ✅

**Summary:**
Enabled FFmpeg video capture support and generated the success video with braitenberg_zoo.xml.

**Build Configuration:**
```bash
cd ~/code/YARS/build
cmake .. -DYARS_USE_CAPTURE_VIDEO=ON
make -j4
```

**Video Generation:**
```bash
./bin/yars --iterations 500 --capture --captureFrameRate 30 --captureDirectory ./final_video ../xml/braitenberg_zoo.xml
```

**Result:**
- Output: `~/code/YARS/build/final_video/yars-0001.mp4`
- Size: 6.9MB
- Frames: 438 at 30fps (~14.6 seconds)
- Codec: H.264 (libx264)
- Encoder: Lavf62.3.100

**Success Criteria Met:**
- ✅ Multiple Braitenberg robots (braitenberg_zoo.xml)
- ✅ Stable physics simulation (500 iterations)
- ✅ Video capture working

---

## Session: 2026-02-02

### Mouse Navigation Overhaul (COMPLETE)

**Status:** Complete

**Summary:**
Completely rewrote the mouse camera controls for intuitive FPS-style navigation on macOS.

**Changes:**
1. **Scroll Wheel Fix** - Reduced sensitivity (0.1 → 0.02), added SDL 2.0.18+ `preciseY` for macOS trackpad
2. **Modifier Key Detection** - Fixed `SDL_KEYUP` handler that was resetting ALL modifiers on ANY key release
3. **FPS-Style Controls** - Replaced orbit-based camera with ego-shooter style controls
4. **SDL Window ID Fix** - Fixed event filtering that was using wrong union field for mouse events

**New Mouse Controls:**
| Action | Control |
|--------|---------|
| Look around (yaw/pitch) | Left drag |
| Strafe (move sideways/up/down) | Middle drag, or Shift+Left |
| Move forward/backward | Right drag (vertical) |
| Move forward/backward | Scroll wheel |

**Files Modified:**
- `src/yars/view/gui/SdlWindow.cpp` - Complete rewrite of mouse handling
- `src/yars/view/gui/SdlWindow.h` - Added roll angle member

**Commits:**
- `fix: Improve scroll wheel sensitivity and macOS support`
- `fix: Fix modifier key detection for macOS mouse controls`
- `feat: Implement FPS-style mouse camera controls`
- `fix: Properly check window ID for different SDL event types`

---

### Hexapod Simulation Fix (COMPLETE)

**Status:** Complete

**Summary:**
Fixed hexapod.xml to work with the built-in Hexapod controller and corrected the gait pattern.

**Changes:**
1. **Controller Path Fix** - Changed from hardcoded Python path to built-in Hexapod controller
2. **Gait Pattern Fix** - Updated C++ controller to match Python controller's gait (robot was walking backwards)
   - Changed phase shifts to: 0, 3π/2, π, 5π/2
   - Changed default period from 10 to 50

**Files Modified:**
- `xml/hexapod.xml` - Use built-in Hexapod controller
- `contrib/controller/Hexapod.cpp` - Fixed gait pattern

**How to Run:**
```bash
cd ~/code/YARS/build
./bin/yars ../xml/hexapod.xml
# Or from anywhere:
./bin/yars --lib ~/code/YARS/build/lib ~/code/YARS/xml/hexapod.xml
```

**Commits:**
- `fix: Update hexapod.xml to use built-in Hexapod controller`
- `fix: Update Hexapod controller to match Python gait pattern`

---

### Documentation & Testing (COMPLETE)

**Status:** Complete

**Summary:**
Updated documentation and tested multiple simulations.

**Changes:**
1. **README.md** - Added mouse controls, CLI options, Python controller example, known issues
2. **hexapod.xml** - Fixed Python controller path for build directory
3. **Tested simulations:**
   - braitenberg_zoo.xml ✅
   - braitenberg_light_source.xml ✅
   - falling_objects.xml ✅
   - muscle.xml ✅
   - braitenberg_trace_projection.xml ✅

**Visual Rendering Verified (code-level):**
- Textures: ground.jpg, wall.jpg, clouds.jpg present
- Materials: YARS/DryGround, YARS/Wall1-3, YARS/CloudySky configured
- Lighting: Ambient (0.5) + point light at (75,150,75)
- Traces: RTSS_TraceLine with vertex colors enabled
- Shadows: SHADOWTYPE_STENCIL_MODULATIVE enabled

**Commits:**
- `docs: Update README with mouse controls, CLI options, and known issues`

---

## Session: 2026-02-01 (Continued)

### Complete Iterator Modernization (COMPLETE)

**Status:** Complete

**Summary:**
Eliminated ALL remaining `std::vector<T>::iterator` declarations from codebase.

**Files Modernized:**
- `YarsXSDGenerator.cpp` - 17 iterators → auto
- `XsdSpecification.cpp` - 5 iterators → auto
- `DataLogging.cpp` - 4 loops → range-based for
- `DataRobot.cpp` - 2 loops modernized
- `XsdChoice.cpp`, `XsdSequence.cpp` - 2 iterators
- `XsdGraph.cpp` - 1 iterator
- `SceneGraphLDRSensor.cpp` - 1 iterator
- `XsdGraphvizExporter.cpp` - 2 iterators

**Result:** 0 `std::vector<T>::iterator` patterns remaining

**Commits:**
- `refactor: Modernize more iterators to range-based for`
- `refactor: Complete iterator modernization across XSD and data layers`

---

### C++17 Features Addition (COMPLETE)

**Status:** Complete

**Features Added:**
1. **string_view** - `StringTokeniser` uses string_view for parameters (avoids copies)
2. **Structured Bindings** - `RobotControllerParameter`, `DataController` use `[key, value]` syntax
3. **if-with-initializer** - `RobotControllerParameter::get()` uses C++17 if-init
4. **emplace_back** - Replace push_back where appropriate

**Files Modernized:**
- `StringTokeniser.h/.cpp`
- `RobotControllerParameter.hpp`
- `DataController.cpp`
- `Robot.cpp`

**Commit:** `refactor: Add C++17 features - string_view, structured bindings, modern idioms`

---

### Smart Pointer Safety (COMPLETE)

**Status:** Complete

**Summary:**
Converted local new/delete pairs to `std::make_unique` for exception safety.

**Files Modernized:**
- `YarsConfiguration.cpp`:
  - `YarsXSDGenerator` → `make_unique`
  - `YarsXSDSaxParser` → `make_unique`
  - `ProgramOptions` → `make_unique`

**Commit:** `refactor: Use unique_ptr for local scope allocations`

---

## Session: 2026-02-01

### Performance Optimizations (COMPLETE)

**Status:** Complete

**Summary:**
Optimized hot path in physics simulation loop for improved performance.

**Changes:**
1. **World.cpp** - Use stepSize parameter instead of calling macro twice
2. **Robot.cpp** - Removed debug cout statement in actuator creation
3. **Robot.cpp** - Merged duplicate object loops in postPhysicsUpdate()
4. **YarsMainControl.cpp** - Cache pause state check to avoid repeated macro calls

**Files Modified:**
- `src/yars/physics/bullet/World.cpp`
- `src/yars/physics/bullet/Robot.cpp`
- `src/yars/main/YarsMainControl.cpp`

---

### Phase 6: Iterator Modernization - Part 1 (COMPLETE)

**Status:** Complete

**Summary:**
Converted old-style std::vector<T>::iterator patterns to modern range-based for loops.

**Files Modernized:**
- `src/yars/util/Directories.cpp` - 9 iterators converted
- `src/yars/logging/LoggingModuleActuator.cpp` - 1 iterator converted
- `src/yars/logging/LoggingModuleSensor.cpp` - 2 iterators converted
- `src/yars/util/PID.cpp` - 2 iterators converted
- `src/yars/util/noise/WhiteNoise.cpp` - 1 iterator converted
- `src/yars/util/noise/GaussianNoise.cpp` - 1 iterator converted
- `src/yars/view/gui/CameraMan.cpp` - 1 iterator converted

**Pattern Changed:**
```cpp
// Before:
for (std::vector<string>::iterator i = vec.begin(); i != vec.end(); i++)
  doSomething(*i);

// After:
for (const auto& item : vec)
  doSomething(item);
```

**Total Iterators Converted:** 17

---

### Testing Infrastructure - Phase 5 (COMPLETE)

**Status:** Complete

**Summary:**
Added Catch2 testing framework with unit and integration tests.

**Test Coverage:**
- **Unit tests:** 44 test cases, 131 assertions
  - P3D: constructors, operators, length, normalize, dot/cross product
  - Quaternion: constructors, conjugate, rotation, euler conversion
  - Matrix: constructors, operators, determinant, transpose, L2 norm
- **Integration tests:** 2 test cases
  - Data singleton
  - YarsConfiguration singleton

**Files Added:**
- `tests/CMakeLists.txt` - Test build configuration
- `tests/unit/test_p3d.cpp` - P3D vector tests
- `tests/unit/test_quaternion.cpp` - Quaternion tests
- `tests/unit/test_matrix.cpp` - Matrix tests
- `tests/integration/test_configuration.cpp` - Configuration tests

**Build & Run:**
```bash
cmake .. -DYARS_BUILD_TESTS=ON
make -j4
ctest --output-on-failure
```

**Commit:** `feat: Add Catch2 testing infrastructure`

---

### Observer Pattern Removal (COMPLETE)

**Status:** Complete

**Summary:**
Replaced Observer/Observable pattern with direct function calls and std::function callbacks.
This removes ~500 lines of indirection and makes the code flow explicit.

**Architecture Change:**
```
BEFORE (Observer pattern):
YarsMainControl → notifyObservers(_m_nextStep) → YarsPhysicsControl::notify() → _model->performOneSimulationStep()

AFTER (direct calls):
YarsMainControl → _ypm->performOneSimulationStep()
```

**Files Removed from Build:**
- `YarsPhysicsControl.cpp` - thin wrapper around YarsPhysicsModel
- `YarsLoggingControl.cpp` - thin wrapper around YarsLoggingModel
- `YarsViewControl.cpp` - thin wrapper around YarsViewModel

**Classes Simplified:**
- `YarsMainControl` - calls models directly, uses callbacks for quit
- `RuntimeControl` - uses `std::function<void()>` callbacks for quit/video
- `SignalHandler` - uses callback instead of Observable
- `KeyHandler` - uses callbacks instead of Observable
- `SdlWindow` - uses callbacks for close/sync events
- `YarsPhysicsModel` - uses callbacks for physics-triggered reset/quit
- `YarsViewModel` - removed Observer inheritance, uses direct calls
- `ConsoleView` - removed Observer inheritance (wasn't used)
- `YarsConfiguration` - removed Observable inheritance (wasn't used)
- `MainLoopThread` - simplified, just wraps YarsMainControl

**Files Modified (23 total):**
- src/yars/main/*.cpp, *.h (5 files)
- src/yars/physics/YarsPhysicsModel.cpp, .h
- src/yars/logging/CMakeLists.txt
- src/yars/view/*.cpp, *.h (4 files)
- src/yars/view/gui/KeyHandler.cpp, .h
- src/yars/view/gui/SdlWindow.cpp, .h
- src/yars/view/console/ConsoleView.cpp, .h
- src/yars/configuration/YarsConfiguration.h
- src/yars/yarsMain.cpp

**Lines Changed:** +448 / -558

**Commit:** `refactor: Remove Observer pattern, use direct function calls and callbacks`

---

### Smart Pointer Migration Summary

**Physics Layer:**
- Robots class → `std::vector<std::unique_ptr<Robot>>`
- Environment class → `std::vector<std::unique_ptr<Object>>`

**View Layer:**
- SceneGraph → unique_ptr for robots, environment, traceLines
- SceneGraphRobotNode → unique_ptr for objects, sensors, actuators
- SceneGraphEnvironmentNode → unique_ptr for lightSources, objects, meshes
- YarsViewModel → unique_ptr for windowManager, newWindows

**Logging Layer:**
- LoggingHandler → unique_ptr for modules, logger

**Iterator Modernization:**
- DataLogging.cpp: 18 FOREACH → range-based for

**FOREACH Elimination:** 56 → 0 active occurrences ✓
**stl_macros.h includes:** 26 → 0 files ✓

**Performance:**
- 50,000 iterations in 1.7 seconds
- Incremental build: <1 second

**Commits (16 total):**
1. `feat: Add Catch2 testing infrastructure` (NEW)
2. `chore: Delete unused Observer pattern files` (NEW)
3. `refactor: Remove Observer pattern, use direct function calls and callbacks`
2. `refactor: Migrate Robots class to std::unique_ptr`
2. `refactor: Migrate Environment class to std::unique_ptr`
3. `refactor: Migrate SceneGraph to std::unique_ptr`
4. `refactor: Migrate SceneGraphRobotNode to std::unique_ptr`
5. `refactor: Migrate SceneGraphEnvironmentNode to std::unique_ptr`
6. `refactor: Migrate YarsViewModel to std::unique_ptr`
7. `refactor: Migrate LoggingHandler to std::unique_ptr`
8. `refactor: Modernize DataLogging.cpp iterators`
9. `chore: Remove unused stl_macros.h includes`

---

## Session: 2026-01-31

### Phase 1: GUI Rendering (COMPLETE)

**Status:** Complete (resolved in later sessions)

**Original Issue:** OGRE 14 + SDL2 window integration crashed on macOS

**Solution (implemented Feb 2026):**
- Fixed YarsErrorHandler null pointer crash
- Proper RTSS shader generation for textured materials
- MaterialManager::resolveMaterialName for all scene graph nodes
- SDL2 + OGRE 14 window integration working on macOS

**Files Modified:**
- `src/yars/view/gui/SdlWindow.cpp` - GL context setup and window params
- `src/yars/view/gui/SdlWindow.h` - Added `_glContext` member, `swapBuffers()` method
- `src/yars/view/gui/OgreHandler.cpp` - Removed Cocoa event pump
- `src/yars/view/YarsViewModel.cpp` - Added swapBuffers call after render

---

### Phase 2.1: Smart Pointer Migration - Robot Class (COMPLETE)

**Status:** Complete

**Changes:**
- `Robot._objects`: `std::vector<Object*>` → `std::vector<std::unique_ptr<Object>>`
- `Robot._actuators`: `std::vector<Actuator*>` → `std::vector<std::unique_ptr<Actuator>>`
- `Robot._sensors`: `std::vector<Sensor*>` → `std::vector<std::unique_ptr<Sensor>>`
- `Robot._controller`: `RobotController*` → `std::unique_ptr<RobotController>`

**Files Modified:**
- `src/yars/physics/bullet/Robot.h` - Smart pointer types, added `<memory>` include
- `src/yars/physics/bullet/Robot.cpp` - Updated creation, destruction, and iteration
- `src/yars/physics/bullet/BulletPhysics.cpp` - Replaced FOREACHF with range-based for
- `src/yars/physics/bullet/Actuator.cpp` - Use `.get()` for raw pointer access
- `src/yars/physics/bullet/Sensor.cpp` - Use `.get()` for raw pointer access

**Benefits:**
- No manual `delete` calls needed in destructor
- Clear ownership semantics
- Memory leak prevention
- Automatic cleanup on exceptions

**Commit:** `refactor: Migrate Robot class to std::unique_ptr`

---

### Phase 2.1: Smart Pointer Migration - Robots Class (COMPLETE)

**Status:** Complete

**Changes:**
- `Robots`: `std::vector<Robot*>` → `std::vector<std::unique_ptr<Robot>>`

**Files Modified:**
- `src/yars/physics/bullet/Robots.h` - Changed inheritance to use unique_ptr
- `src/yars/physics/bullet/Robots.cpp` - Replaced FOREACH macros with range-based for loops
- `src/yars/physics/bullet/BulletPhysics.cpp` - Updated Robot iteration

**Commit:** `refactor: Migrate Robots class to std::unique_ptr`

---

### Phase 2.1: Smart Pointer Migration - Environment Class (COMPLETE)

**Status:** Complete

**Changes:**
- `Environment`: `std::vector<Object*>` → `std::vector<std::unique_ptr<Object>>`

**Files Modified:**
- `src/yars/physics/bullet/Environment.h` - Changed inheritance to use unique_ptr
- `src/yars/physics/bullet/Environment.cpp` - Range-based for loops, make_unique
- `src/yars/physics/bullet/BulletPhysics.cpp` - Updated environment iteration

**Commit:** `refactor: Migrate Environment class to std::unique_ptr`

---

### Phase 2.2: Smart Pointer Migration - View Layer (COMPLETE)

**Status:** Complete

**SceneGraph:**
- `_robots`: `std::vector<SceneGraphRobotNode*>` → `std::vector<std::unique_ptr<SceneGraphRobotNode>>`
- `_environment`: `SceneGraphEnvironmentNode*` → `std::unique_ptr<SceneGraphEnvironmentNode>`
- `_traceLines`: `std::vector<SceneGraphTraceLineObject*>` → `std::vector<std::unique_ptr<SceneGraphTraceLineObject>>`

**SceneGraphRobotNode:**
- `_objects`, `_sensors`, `_actuators`: All migrated to `std::vector<std::unique_ptr<SceneGraphObjectNode>>`

**SceneGraphEnvironmentNode:**
- `_lightSources`, `_objects`, `_meshes`: All migrated to `std::vector<std::unique_ptr<SceneGraphObjectNode>>`

**Files Modified:**
- `src/yars/view/gui/SceneGraph.h/.cpp`
- `src/yars/view/gui/SceneGraphRobotNode.h/.cpp`
- `src/yars/view/gui/SceneGraphEnvironmentNode.h/.cpp`

**Memory Leaks Fixed:**
- SceneGraph had no destructor
- SceneGraphRobotNode destructor was commented out
- SceneGraphEnvironmentNode didn't delete _lightSources

**Commits:**
- `refactor: Migrate SceneGraph to std::unique_ptr`
- `refactor: Migrate SceneGraphRobotNode to std::unique_ptr`
- `refactor: Migrate SceneGraphEnvironmentNode to std::unique_ptr`

---

### Phase 2.2: Smart Pointer Migration - YarsViewModel (COMPLETE)

**Status:** Complete

**Changes:**
- `_windowManager`: `std::vector<SdlWindow*>` → `std::vector<std::unique_ptr<SdlWindow>>`
- `_newWindows`: `std::vector<SdlWindow*>` → `std::vector<std::unique_ptr<SdlWindow>>`

**Files Modified:**
- `src/yars/view/YarsViewModel.h` - Smart pointer types
- `src/yars/view/YarsViewModel.cpp` - Range-based for loops, erase-remove idiom

**FOREACH Reduction:** 56 → 46 occurrences (10 removed)

**Commit:** `refactor: Migrate YarsViewModel to std::unique_ptr`

---

## Previously Completed (Earlier Sessions)

### Phase 0: Build System (COMPLETE)
- CMake updated to 3.16+ with C++17
- Boost linking fixed for modern Boost 1.89+
- Build succeeds without visualization

### Phase 2: Boost Replacement (COMPLETE)
- `boost::thread` → `std::thread`
- `boost::function` → `std::function`
- `boost::filesystem` → `std::filesystem`
- `boost::date_time` → `std::chrono`

### Phase 3: Code Modernization (74% COMPLETE)
- NULL → nullptr (509 occurrences)
- Added override specifiers
- Iterator modernization (378 → 98 remaining)

---

## Validation

All changes verified with:
```bash
cd ~/code/YARS/build
cmake .. -DYARS_USE_CAPTURE_VIDEO=ON && make -j4

# Physics validation
./bin/yars --iterations 1000 ../xml/braitenberg_logging.xml

# GUI rendering validation
./bin/yars ../xml/braitenberg.xml

# Video capture validation
./bin/yars --iterations 500 --capture --captureFrameRate 30 --captureDirectory ./videos ../xml/braitenberg_zoo.xml
```

Physics simulation, GUI rendering, and video capture all working correctly.

---

## Related Documents

- [[refactoring_plan]] - Full refactoring roadmap
- [[analysis]] - Codebase analysis
