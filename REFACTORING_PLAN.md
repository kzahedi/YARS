# YARS Comprehensive Refactoring Plan

## Overview
This document provides a step-by-step execution plan for refactoring YARS from a 2004-2019 legacy codebase to a modern C++20 architecture. The plan is designed to be executable without circular dependencies or backtracking.

## Critical Success Factors
1. **Maintain Functionality**: Each step must preserve existing functionality
2. **Incremental Progress**: Small, testable changes that can be validated
3. **Clear Dependencies**: No circular dependencies between tasks
4. **Validation Points**: Clear criteria for task completion

## Phase 0: Preparation and Baseline (Week 0)

### Task 0.1: Create Testing Baseline
**Issue**: #001
**Priority**: CRITICAL
**Dependencies**: None

**Actions**:
1. Document current working commands and expected outputs
2. Create regression test suite for critical paths
3. Record performance benchmarks
4. Create validation scripts

**Validation**:
```bash
./bin/yars --iterations 1000 ../xml/braitenberg.xml
# Should complete without errors
# Record output for comparison
```

**Files Created**:
- `tests/baseline/braitenberg_output.txt`
- `tests/baseline/performance_metrics.json`
- `tests/validate_refactoring.sh`

### Task 0.2: Setup Development Environment
**Issue**: #002
**Priority**: CRITICAL
**Dependencies**: None

**Actions**:
1. Create development branch structure
2. Setup CI/CD pipeline
3. Configure code formatting tools
4. Create backup of working state

**Commands**:
```bash
git checkout -b refactoring/main
git checkout -b refactoring/phase1-observer-removal
```

**Files Created**:
- `.clang-format`
- `.github/workflows/ci.yml`
- `tools/format_code.sh`

## Phase 1: Observer Pattern Removal (Week 1-2)

### Task 1.1: Map Observer Dependencies
**Issue**: #003
**Priority**: HIGH
**Dependencies**: Task 0.1, 0.2

**Actions**:
1. Create dependency graph of all Observer usage
2. Identify message types and their consumers
3. Document current message flow
4. Plan direct call replacements

**Analysis Script**:
```bash
grep -r "Observable\|Observer\|notifyObservers" src/ > observer_usage.txt
grep -r "ObservableMessage" src/ | grep "new" > message_creation.txt
```

**Files Created**:
- `docs/observer_dependency_graph.dot`
- `docs/message_flow_current.md`
- `docs/direct_call_mapping.md`

### Task 1.2: Create New Control Interface
**Issue**: #004
**Priority**: HIGH
**Dependencies**: Task 1.1

**Actions**:
1. Design `SimulationController` interface
2. Create direct method signatures
3. Implement facade pattern for transition
4. Add compatibility layer

**New Files**:
```cpp
// src/yars/control/SimulationController.h
class SimulationController {
public:
    void start();
    void stop();
    void step();
    void reset();
    void setSpeed(double speed);
    
    // Direct component access
    PhysicsEngine* getPhysics();
    Renderer* getRenderer();
    Logger* getLogger();
};
```

**Validation**:
- New interface compiles
- Can be instantiated without Observer

### Task 1.3: Replace Observable in YarsMainControl
**Issue**: #005
**Priority**: HIGH
**Dependencies**: Task 1.2

**Actions**:
1. Remove Observable inheritance from YarsMainControl
2. Replace notifyObservers with direct calls
3. Update constructor and initialization
4. Test with simple scenarios

**Changes**:
```cpp
// OLD: src/yars/main/YarsMainControl.cpp
class YarsMainControl : public Observable, public Observer {
    void update() { notifyObservers(new ObservableMessage(YARS_PHYSICS_STEP)); }
}

// NEW: src/yars/main/YarsMainControl.cpp
class YarsMainControl {
    SimulationController* controller;
    void update() { controller->step(); }
}
```

**Validation**:
```bash
make YarsMainControl.o
./tests/validate_refactoring.sh --component main
```

### Task 1.4: Remove Observer from Physics
**Issue**: #006
**Priority**: HIGH
**Dependencies**: Task 1.3

**Actions**:
1. Remove Observer inheritance from YarsPhysicsControl
2. Replace message handling with direct method calls
3. Update physics loop to use callbacks
4. Test physics simulation

**Validation**:
```bash
./bin/yars --no-gui --iterations 100 ../xml/braitenberg.xml
# Physics should run without Observer
```

### Task 1.5: Remove Observer from View
**Issue**: #007
**Priority**: MEDIUM
**Dependencies**: Task 1.3

**Actions**:
1. Remove Observer from YarsViewControl
2. Replace message-based updates with direct rendering calls
3. Update camera controls to use callbacks
4. Test visualization

**Validation**:
```bash
./bin/yars --iterations 100 ../xml/braitenberg.xml
# GUI should display without Observer
```

### Task 1.6: Remove Observer from Logging
**Issue**: #008
**Priority**: LOW
**Dependencies**: Task 1.3

**Actions**:
1. Remove Observer from YarsLoggingControl
2. Implement direct logging interface
3. Update loggers to use callbacks
4. Test data logging

**Validation**:
```bash
./bin/yars --log-csv output.csv ../xml/braitenberg_logging.xml
# Should produce valid CSV output
```

### Task 1.7: Clean Up Observer Code
**Issue**: #009
**Priority**: LOW
**Dependencies**: Tasks 1.3-1.6

**Actions**:
1. Remove Observable.h, Observer.h, ObservableMessage.h
2. Remove all message type definitions
3. Update CMakeLists.txt
4. Full rebuild and test

**Validation**:
```bash
find src/ -name "*Observable*" -o -name "*Observer*" | wc -l
# Should return 0
make clean && make
./tests/validate_refactoring.sh --full
```

## Phase 2: C++ Modernization (Week 3-4)

### Task 2.1: Upgrade to C++20
**Issue**: #010
**Priority**: HIGH
**Dependencies**: Phase 1 complete

**Actions**:
1. Update CMakeLists.txt to require C++20
2. Update compiler flags
3. Fix compilation errors
4. Run full test suite

**Changes**:
```cmake
# CMakeLists.txt
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

**Validation**:
```bash
cmake -DCMAKE_CXX_STANDARD=20 ..
make -j8
```

### Task 2.2: Replace Boost Filesystem
**Issue**: #011
**Priority**: HIGH
**Dependencies**: Task 2.1

**Actions**:
1. Replace `boost::filesystem` with `std::filesystem`
2. Update all path operations
3. Update file existence checks
4. Test file operations

**Changes**:
```cpp
// OLD
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

// NEW
#include <filesystem>
namespace fs = std::filesystem;
```

**Files Affected**:
- `src/yars/util/FileSystemOperations.cpp`
- `src/yars/util/Directories.cpp`

### Task 2.3: Replace Boost Thread
**Issue**: #012
**Priority**: HIGH
**Dependencies**: Task 2.1

**Actions**:
1. Replace `boost::thread` with `std::thread`
2. Replace `boost::mutex` with `std::mutex`
3. Update thread management
4. Test multi-threading

**Files Affected**:
- `src/yars/main/MainLoopThread.cpp`
- `src/yars/view/gui/GuiMutex.cpp`

### Task 2.4: Replace Boost Program Options
**Issue**: #013
**Priority**: MEDIUM
**Dependencies**: Task 2.1

**Actions**:
1. Integrate CLI11 library
2. Rewrite ProgramOptions class
3. Update command-line parsing
4. Test all command-line options

**Validation**:
```bash
./bin/yars --help
./bin/yars --version
./bin/yars --iterations 100 --no-gui ../xml/braitenberg.xml
```

### Task 2.5: Modern Memory Management
**Issue**: #014
**Priority**: MEDIUM
**Dependencies**: Task 2.1

**Actions**:
1. Replace raw pointers with smart pointers
2. Use `std::unique_ptr` for ownership
3. Use `std::shared_ptr` for shared resources
4. Fix memory leaks

**Example**:
```cpp
// OLD
DataRobot* robot = new DataRobot();
delete robot;

// NEW
auto robot = std::make_unique<DataRobot>();
```

## Phase 3: Configuration System Overhaul (Week 5-6)

### Task 3.1: Design JSON Schema
**Issue**: #015
**Priority**: HIGH
**Dependencies**: Phase 2 complete

**Actions**:
1. Create JSON schema for robot configurations
2. Design conversion from XML to JSON
3. Create schema validator
4. Document new format

**Files Created**:
- `schemas/robot.schema.json`
- `tools/xml_to_json_converter.py`
- `docs/configuration_format.md`

### Task 3.2: Implement JSON Parser
**Issue**: #016
**Priority**: HIGH
**Dependencies**: Task 3.1

**Actions**:
1. Integrate nlohmann/json
2. Create JSON configuration loader
3. Implement backward compatibility
4. Test configuration loading

**New Files**:
```cpp
// src/yars/configuration/JsonConfigLoader.h
class JsonConfigLoader {
    std::unique_ptr<Data> load(const std::filesystem::path& file);
    void validate(const nlohmann::json& config);
};
```

### Task 3.3: Simplify Factory Pattern
**Issue**: #017
**Priority**: MEDIUM
**Dependencies**: Task 3.2

**Actions**:
1. Replace 80+ factory classes with template factory
2. Use registration pattern
3. Simplify object creation
4. Test all object types

**New Pattern**:
```cpp
template<typename T>
class Factory {
    using Creator = std::function<std::unique_ptr<T>()>;
    std::map<std::string, Creator> creators;
public:
    void register(const std::string& type, Creator creator);
    std::unique_ptr<T> create(const std::string& type);
};
```

### Task 3.4: Migrate XML Configurations
**Issue**: #018
**Priority**: LOW
**Dependencies**: Task 3.2

**Actions**:
1. Convert all XML files to JSON
2. Validate converted files
3. Update examples
4. Update documentation

**Script**:
```bash
for xml in xml/*.xml; do
    python tools/xml_to_json_converter.py $xml ${xml%.xml}.json
done
```

## Phase 4: Graphics Modernization (Week 7-8)

### Task 4.1: Evaluate Graphics Options
**Issue**: #019
**Priority**: HIGH
**Dependencies**: Phase 3 complete

**Actions**:
1. Benchmark Ogre3D 14.x integration effort
2. Prototype modern OpenGL renderer
3. Evaluate Vulkan feasibility
4. Make technology decision

**Decision Matrix**:
| Option | Effort | Performance | Maintainability |
|--------|--------|-------------|-----------------|
| Ogre3D 14.x | Medium | Good | Medium |
| OpenGL 4.6 | High | Good | High |
| Vulkan | Very High | Excellent | Medium |

### Task 4.2: Implement Chosen Graphics Solution
**Issue**: #020
**Priority**: HIGH
**Dependencies**: Task 4.1

**If Modern OpenGL chosen**:
1. Setup GLFW window management
2. Implement basic renderer
3. Port existing shaders
4. Implement camera system

**New Files**:
```cpp
// src/yars/graphics/ModernRenderer.h
class ModernRenderer {
    void initialize();
    void render(const Scene& scene);
    void shutdown();
};
```

### Task 4.3: Migrate Materials and Shaders
**Issue**: #021
**Priority**: MEDIUM
**Dependencies**: Task 4.2

**Actions**:
1. Convert Ogre materials to new format
2. Update shaders for modern pipeline
3. Implement texture loading
4. Test all visual elements

### Task 4.4: Implement UI System
**Issue**: #022
**Priority**: LOW
**Dependencies**: Task 4.2

**Actions**:
1. Integrate Dear ImGui
2. Replace text overlays
3. Add debug visualization
4. Create control panels

## Phase 5: Testing and Optimization (Week 9-10)

### Task 5.1: Create Comprehensive Test Suite
**Issue**: #023
**Priority**: HIGH
**Dependencies**: Phase 4 complete

**Actions**:
1. Setup Google Test
2. Create unit tests for core components
3. Create integration tests
4. Setup continuous testing

**Test Structure**:
```
tests/
├── unit/
│   ├── physics/
│   ├── configuration/
│   └── graphics/
├── integration/
│   ├── simulation/
│   └── controllers/
└── performance/
    └── benchmarks/
```

### Task 5.2: Performance Optimization
**Issue**: #024
**Priority**: MEDIUM
**Dependencies**: Task 5.1

**Actions**:
1. Profile with perf/VTune
2. Optimize hot paths
3. Implement parallel physics
4. Optimize rendering

**Benchmarks**:
```bash
./tools/benchmark.sh --baseline old_yars --new ./bin/yars
```

### Task 5.3: Documentation Update
**Issue**: #025
**Priority**: LOW
**Dependencies**: All phases complete

**Actions**:
1. Update README
2. Create migration guide
3. Update API documentation
4. Create tutorials

## Validation Checkpoints

### After Each Phase:
1. **Compilation**: Clean build without warnings
2. **Functionality**: All example scenarios run
3. **Performance**: No regression from baseline
4. **Memory**: No leaks (valgrind clean)

### Final Validation:
```bash
# Full test suite
./tests/run_all_tests.sh

# Performance comparison
./tools/benchmark.sh --full

# Memory check
valgrind --leak-check=full ./bin/yars --iterations 1000 ../xml/braitenberg.xml

# All examples
for example in examples/*.json; do
    ./bin/yars --validate $example
done
```

## Risk Mitigation

### Rollback Strategy
1. Each phase in separate branch
2. Merge only after validation
3. Keep compatibility layers temporarily
4. Document breaking changes

### Critical Path Items
1. Observer removal - highest risk, highest impact
2. Graphics migration - high effort, user-visible
3. Configuration change - affects all users

### Contingency Plans
1. **Observer removal fails**: Implement adapter pattern
2. **Graphics too complex**: Keep Ogre3D, upgrade version
3. **Performance regression**: Profile and optimize incrementally

## Success Metrics

### Technical Metrics
- [ ] 50% reduction in compilation time
- [ ] 30% performance improvement
- [ ] 0 memory leaks
- [ ] 100% test coverage for core components

### Code Quality Metrics
- [ ] Cyclomatic complexity < 10
- [ ] No circular dependencies
- [ ] Clear separation of concerns
- [ ] Modern C++ idioms throughout

### User Experience Metrics
- [ ] Simpler configuration format
- [ ] Better error messages
- [ ] Faster startup time
- [ ] Responsive UI

## Knowledge Base

### Critical Findings (Updated During Refactoring)
1. **Observer Pattern Entanglement**: [Document specific issues here]
2. **Performance Bottlenecks**: [Document findings here]
3. **Architecture Decisions**: [Document rationale here]
4. **Gotchas and Workarounds**: [Document solutions here]

### Tools and Scripts
- `tools/analyze_dependencies.py`: Generate dependency graphs
- `tools/benchmark.sh`: Performance comparison
- `tools/validate_refactoring.sh`: Regression testing
- `tools/memory_check.sh`: Memory leak detection

## Appendix: Issue Template

Each GitHub issue should contain:
1. **Clear Description**: What and why
2. **Acceptance Criteria**: Definition of done
3. **Dependencies**: Blocking issues
4. **Validation Steps**: How to verify
5. **Rollback Plan**: If something goes wrong

This plan provides a clear, linear path through the refactoring without circular dependencies or confusion.