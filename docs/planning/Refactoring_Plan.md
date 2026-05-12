# YARS Comprehensive Refactoring Plan

## Executive Summary

**Project:** YARS (Yet Another Robot Simulator)
**Codebase:** ~56,000 LOC across 535 files
**Branch:** `modernization-2026`
**Base Commit:** `48d9fe4` (April 24, 2019)
**Backup:** `backup-modernization-2025`

---

## Current State (February 2026) - ALL PHASES COMPLETE ✅

### Completed Work
- [x] CMake updated to 3.16+ with C++17
- [x] Boost **completely removed** - replaced with C++17 std:: and CLI11
- [x] NULL → nullptr (509 occurrences)
- [x] Added override specifiers to virtual methods
- [x] Iterator modernization (100% complete)
- [x] Physics simulation works (GUI and headless)
- [x] OGRE 14 + SDL2 GUI rendering fixed
- [x] Video capture working (H.264/MP4)
- [x] Observer pattern removed (direct calls + callbacks)
- [x] Smart pointer migration complete
- [x] Unit testing with Catch2 (60 test cases)
- [x] Linux build support added (untested)

### Boost Status: REMOVED ✅
- `boost::program_options` → **CLI11** (header-only)
- `boost::circular_buffer` → **Custom CircularBuffer<T>** template
- `boost::filesystem` → `std::filesystem`
- `boost::thread` → `std::thread`

### Current Focus
- **Linux Port Testing** - See [[Linux_Port]] for details

---

## Success Criteria

**The project is complete when we can generate a video showing:**
- 5-6 Braitenberg robots from top-down view
- Properly rendered textures (ground, walls, robots)
- Visible sensor visualizations
- Robot movement traces
- Stable physics simulation

**Test Configuration:** `xml/braitenberg_zoo.xml`

---

## Phase 1: Fix GUI Rendering (BLOCKING)

**Status:** BLOCKED (requires OGRE 14 debugging)
**Priority:** CRITICAL
**Risk:** High
**Estimated Files:** 3-5

### Problem Analysis

The crash occurs in the OGRE render loop when using an external SDL2 window handle on macOS:

```
SdlWindow creates SDL_Window
  → Gets native window handle (Cocoa NSWindow/NSView)
  → Passes to OGRE as "externalWindowHandle"
  → OGRE 14 CocoaWindow wraps it
  → First renderOneFrame() → SIGTRAP (exit code 133)
```

### Attempted Solutions (2026-01-31)

**1. currentGLContext approach** - Let SDL create GL context, pass to OGRE
- Result: Still crashes on renderOneFrame()
- Files modified: SdlWindow.cpp

**2. externalGLContext approach** - Pass SDL's GL context pointer to OGRE
- Result: Still crashes on renderOneFrame()
- Params: externalGLContext, externalGLControl, externalWindowHandle

**3. Native OGRE Window** - Let OGRE create its own Cocoa window
- Result: OGRE throws UnimplementedException
- Message: "Builtin Window creation broken. Use an external Window"

**4. Various GL configurations**
- OpenGL 3.2, 3.3, 4.1 Core Profile
- Different SDL GL attributes (depth, stencil, sRGB)
- Result: All crash with exit code 133

### Root Cause Analysis

OGRE 14's `OgreOSXCocoaWindow.mm` has issues with:
1. External window handle integration on modern macOS
2. GL context sharing between SDL and OGRE
3. Builtin window creation is explicitly broken (throws exception)

### Workaround

Use `--nogui` mode for physics simulation validation:
```bash
./bin/yars --xml ../xml/braitenberg.xml --nogui
```

### Potential Solutions (Future)

1. **Debug OGRE 14** - Step through CocoaWindow.mm to find exact crash point
2. **Use Metal** - Switch to Metal render system (requires shader updates)
3. **Downgrade OGRE** - Use OGRE 13.x which may have better macOS support
4. **SDL3** - Try SDL3 which has better macOS integration

### Solution B: Metal Render System (Alternative)

Switch from GL3Plus to Metal on macOS.

**Files to Modify:**

1. `src/yars/view/gui/OgreHandler.cpp`
   - Load Metal render system instead of GL3Plus
   - Update render system configuration

2. `CMakeLists.txt`
   - Link against OGRE Metal plugin

### Missing Textures (to fix after crash resolved)
- GreenSkin.jpg, humanskinfeb.jpg
- snake1.jpg, snake2.jpg, snake3.jpg
- skin.jpg, dirt3.jpg, DirtGround.png
- GrassGreenTexture0002.jpg, flaretrail.jpg, flare_alpha.dds

### Validation
- [x] Window opens without crash (verified 2026-05-11 — OGRE rendering working)
- [x] Basic shapes render correctly (verified 2026-05-11)
- [x] Camera controls work (verified 2026-05-11)
- [x] Multiple windows work (if supported) (verified 2026-05-11)

---

## Phase 2: Smart Pointer Migration

**Status:** IN PROGRESS (Phase 2.1 Robot complete)
**Priority:** HIGH
**Risk:** Low-Medium
**Estimated Files:** 15-20

### Ownership Analysis

| Container | Owns | Current | Target |
|-----------|------|---------|--------|
| `World` | `Robot*` | raw pointer | `unique_ptr<Robot>` |
| `Robot` | `Sensor*` | **DONE** unique_ptr | `unique_ptr<Sensor>` |
| `Robot` | `Actuator*` | **DONE** unique_ptr | `unique_ptr<Actuator>` |
| `Robot` | `Object*` | **DONE** unique_ptr | `unique_ptr<Object>` |
| `Robot` | `RobotController*` | **DONE** unique_ptr | `unique_ptr<RobotController>` |
| `Environment` | `Object*` | raw pointer | `unique_ptr<Object>` |
| `SceneGraph` | `SceneNode*` | raw pointer | `unique_ptr<SceneNode>` |
| `Data*` hierarchy | child `Data*` | raw pointer | `shared_ptr<Data*>` |

### Implementation Order

#### 2.1 Physics Layer - Robot (COMPLETE 2026-01-31)

**Completed Changes:**
- Robot.h/cpp: All vectors now use unique_ptr
- BulletPhysics.cpp: Updated to use auto iterators
- Actuator.cpp/Sensor.cpp: Use .get() for raw pointer access
- FOREACH macros replaced with range-based for loops

**Files:**
- `src/yars/physics/bullet/World.h/.cpp` - TODO
- `src/yars/physics/bullet/Robot.h/.cpp` - DONE
- `src/yars/physics/bullet/Sensor.h/.cpp` - Updated
- `src/yars/physics/bullet/Actuator.h/.cpp` - Updated

**Pattern:**
```cpp
// Before
class Robot {
    std::vector<Sensor*> _sensors;
    ~Robot() {
        for (auto* s : _sensors) delete s;
    }
};

// After
class Robot {
    std::vector<std::unique_ptr<Sensor>> _sensors;
    // Destructor auto-cleans
};
```

#### 2.2 View Layer

**Files:**
- `src/yars/view/gui/SceneGraph.h/.cpp`
- `src/yars/view/gui/SceneGraphRobotNode.h/.cpp`
- `src/yars/view/gui/SceneGraphObjectNode.h/.cpp`

#### 2.3 Configuration Layer

**Files:**
- `src/yars/configuration/data/DataNode.h`
- All `Data*.h/.cpp` files (200+ files)

**Note:** Configuration layer uses shared ownership patterns (macros, references). Use `shared_ptr` here.

### Validation
- [ ] No memory leaks (valgrind/AddressSanitizer)
- [ ] All simulations run correctly
- [ ] No double-free crashes

> **Audit status (2026-05-12):** see
> [`memory-safety-audit-status.md`](./memory-safety-audit-status.md). 17 standalone configs
> were exercised under ASan + UBSan in `build-asan/`. No double-free, no use-after-free,
> no leaks (LSan deferred to Linux CI). Four pre-existing defects were uncovered and
> routed to follow-up fix proposals; the smart-pointer migration itself produced a clean
> baseline.

---

## Phase 3: Configuration System Refactoring

**Status:** NOT STARTED
**Priority:** MEDIUM
**Risk:** High
**Estimated LOC Reduction:** 10,000-15,000

### Current Problems

1. **Massive duplication** - 258 files, 31,123 LOC (55% of codebase)
2. **Repetitive patterns** - Each `Data*` class has similar:
   - Constructor
   - `add(DataParseElement*)` - SAX parsing
   - `createXsd(XsdSpecification*)` - Schema generation
   - `copy()` - Deep copy
   - `resetTo()` - State reset

### Solution: Template + Code Generation

#### 3.1 Create Base Template Infrastructure

**New Files:**
- `src/yars/configuration/DataBinding.h` - Attribute binding templates
- `src/yars/configuration/DataContainer.h` - Generic container template

```cpp
// DataBinding.h
template<typename T>
struct Attribute {
    const char* name;
    T* target;
    bool required;
};

template<typename... Attrs>
class DataBinder {
public:
    void bind(DataParseElement* element, Attrs&... attrs);
    void createXsd(XsdSpecification* spec, Attrs&... attrs);
};
```

#### 3.2 Consolidate Shape Classes

**Before:** Separate classes for each shape
- `DataBox.h/.cpp`
- `DataSphere.h/.cpp`
- `DataCylinder.h/.cpp`
- `DataCapsule.h/.cpp`
- `DataPly.h/.cpp`

**After:** Single generic class
```cpp
// DataShape.h
enum class ShapeType { Box, Sphere, Cylinder, Capsule, Ply, Composite };

class DataShape : public DataNode {
    ShapeType _type;
    P3D _dimensions;  // width/height/depth or radius
    std::string _meshFile;  // for Ply
    // ... common attributes
};
```

#### 3.3 Code Generator (Long-term)

Create a Python tool that parses the XSD schema and generates C++ classes.

**Input:** `yars.xsd`
**Output:** `generated/Data*.h`, `generated/Data*.cpp`

### Validation
- [ ] All XML configs parse correctly
- [ ] XSD generation produces valid schemas
- [ ] No regression in simulation behavior

> **Audit status (2026-05-12):** see
> [`xml-xsd-validation-status.md`](./xml-xsd-validation-status.md). 27/27 corpus configs
> validate against the regenerated XSD (after one hand-edit to fix a generator-emitted
> non-deterministic content model); 18/18 standalone configs parse cleanly via the new
> `make test_xml_parse` driver; the braitenberg behavior-regression diff is bit-identical
> on the rows that land. Three follow-up fix proposals were identified.

---

## Phase 4: Observer Pattern Modernization

**Status:** PARTIAL
**Priority:** MEDIUM
**Risk:** High
**Estimated Files:** 20-30

### Current Architecture

```
Observable (base class)
    │
    ├── YarsMainControl
    ├── YarsPhysicsControl
    ├── YarsLoggingControl
    ├── YarsViewControl
    └── YarsViewModel

Observer (interface)
    │
    ├── YarsPhysicsControl
    ├── YarsLoggingControl
    ├── ConsoleView
    └── RuntimeControl
```

### Completed
- Modernized Observable class (std::vector, range-based for, nullptr checks)
- Mapped all Observer relationships

### Proposed: Event Bus with std::function

```cpp
// EventBus.h
enum class EventType {
    SimulationStart,
    SimulationStep,
    SimulationEnd,
    Reset,
    Quit
};

struct Event {
    EventType type;
    std::any data;  // Optional payload
};

class EventBus {
public:
    using Handler = std::function<void(const Event&)>;

    void subscribe(EventType type, Handler handler);
    void emit(const Event& event);

private:
    std::unordered_map<EventType, std::vector<Handler>> _handlers;
};

// Global instance
extern EventBus& events();
```

### Migration Strategy

1. **Add EventBus alongside Observer** (don't remove yet)
2. **Migrate one subscriber at a time**
3. **Remove Observer when all migrated**

### Files to Modify

- `src/yars/util/Observable.h/.cpp` → Eventually remove
- `src/yars/util/Observer.h` → Eventually remove
- `src/yars/main/YarsMainControl.cpp`
- `src/yars/physics/YarsPhysicsControl.cpp`
- `src/yars/logging/YarsLoggingControl.cpp`
- `src/yars/view/YarsViewControl.cpp`

---

## Phase 5: Testing Infrastructure

**Status:** NOT STARTED
**Priority:** MEDIUM
**Risk:** Low

### Framework Selection

**Recommendation:** Catch2 (header-only, modern C++)

### Test Categories

1. **Unit Tests**
   - Configuration parsing
   - Math types (P3D, Matrix3x3, Quaternion)
   - Sensor calculations

2. **Integration Tests**
   - Full simulation step
   - Controller loading
   - Logging output

3. **Regression Tests**
   - Compare simulation output against baseline

### Directory Structure

```
YARS/
├── tests/
│   ├── unit/
│   │   ├── test_p3d.cpp
│   │   ├── test_quaternion.cpp
│   │   └── test_xml_parsing.cpp
│   ├── integration/
│   │   ├── test_simulation_step.cpp
│   │   └── test_controller_loading.cpp
│   └── CMakeLists.txt
```

### CMake Integration

```cmake
# tests/CMakeLists.txt
find_package(Catch2 REQUIRED)

add_executable(yars_tests
    unit/test_p3d.cpp
    unit/test_quaternion.cpp
)
target_link_libraries(yars_tests PRIVATE Catch2::Catch2WithMain yars_core)

include(CTest)
include(Catch)
catch_discover_tests(yars_tests)
```

---

## Phase 6: Remaining Iterator Modernization

**Status:** 74% COMPLETE
**Priority:** LOW
**Risk:** Low
**Remaining:** ~98 occurrences in 51 files

### Completed Batches

- **Batch 1** (21 files): DataRobot.cpp, YarsLoggingModel.cpp, DataController.cpp, DataEnvironment.cpp,
  DataLogging.cpp, DataMacro.cpp, DataMacroInstance.cpp, DataComposite.cpp,
  all Logger files, physics Robot/Robots/Environment/BulletPhysics, GUI SceneGraph files
- **Batch 2** (8 files): YarsViewModel.cpp, DataTraces.cpp, DataScreens.cpp,
  Composite.cpp, Actuator.cpp, Sensor.cpp, DataMacros.cpp, DataRecording.cpp
- **Batch 3**: DataRobotSimulationDescription.cpp (24 occurrences)

### Files with Most Remaining Occurrences

| File | Count |
|------|-------|
| `YarsXSDGenerator.cpp` | 17 |
| `Directories.cpp` | 9 |
| Various `Data*.cpp` | 3-5 each |

### Approach

Continue converting as encountered during other refactoring work.

---

## Phase 7: Final Validation

**Status:** NOT STARTED
**Priority:** LOW (after Phase 1)

### Tasks

1. Run `braitenberg_zoo.xml` configuration
2. Capture frames
3. Generate video
4. Compare with expected output

---

## Execution Timeline

| Week | Phase | Deliverable |
|------|-------|-------------|
| 1 | Phase 1 | GUI renders without crash |
| 2 | Phase 2.1 | Physics layer uses smart pointers |
| 3 | Phase 2.2 | View layer uses smart pointers |
| 4-5 | Phase 2.3 | Config layer uses smart pointers |
| 6 | Phase 5 | Test framework in place |
| 7-8 | Phase 3.1-3.2 | Config templates + shape consolidation |
| 9-10 | Phase 4 | EventBus replaces Observer |
| Ongoing | Phase 6 | Iterator cleanup |
| Future | Phase 3.3 | Code generator for Data* classes |

---

## Validation Checkpoints

After each phase:

1. **Build Check**
   ```bash
   cd ~/code/YARS/build && cmake .. && make -j4
   ```

2. **Headless Simulation**
   ```bash
   ./bin/yars --xml ../xml/braitenberg.xml --nogui
   ```

3. **GUI Simulation** (after Phase 1)
   ```bash
   ./bin/yars --xml ../xml/braitenberg.xml
   ```

4. **Logging Validation**
   ```bash
   ./bin/yars --iterations 1000 --xml ../xml/braitenberg_logging.xml
   diff braitenberg-*.csv ../reference_logfile.csv
   ```

5. **Memory Check** (after Phase 2)
   ```bash
   # macOS
   leaks --atExit -- ./bin/yars --xml ../xml/braitenberg.xml --nogui
   ```

---

## Risk Mitigation

1. **Incremental commits** - Commit after each working change
2. **Feature branches** - Each phase on separate branch
3. **Backup branch** - `backup-modernization-2025` exists
4. **Parallel development** - Keep `--nogui` mode working throughout

---

## Build Commands Reference

```bash
# Build
cd ~/code/YARS/build
cmake .. && make -j4

# Run (once built)
./bin/yars --xml ../xml/braitenberg.xml

# Check dependencies (macOS)
brew list | grep -E "boost|xerces|bullet|sdl2|ogre"

# Install dependencies if needed
brew install boost xerces-c bullet sdl2 ogre
```

---

## Git Workflow

```bash
# We're on branch: modernization-2026
# Previous attempt backed up to: backup-modernization-2025

# Commit often after each successful change
git add -A && git commit -m "description"

# If something goes wrong, we can always reset
git checkout 48d9fe4 -- <file>
```

---

## Related Documents

- [[analysis]] - Full codebase analysis
- [[CLAUDE]] - Project-specific Claude Code instructions
