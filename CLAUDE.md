# YARS - Yet Another Robot Simulator

## Project Status
**Refactoring In Progress** - Modernizing 2004-2019 codebase to C++20

## Critical Knowledge Base

### Architecture Overview
- **Language**: C++11 (targeting C++20)
- **Physics**: Bullet Physics 3
- **Graphics**: Ogre3D 13.6.4 (requires fixing)
- **Windowing**: SDL2
- **Configuration**: XML with Xerces-C++ (targeting JSON)
- **Lines of Code**: ~100,000+

### Primary Refactoring Target: Observer Pattern
The Observer pattern is used pervasively throughout YARS with 30+ message types, creating:
- Complex, hard-to-follow control flow
- Debugging difficulties
- Performance overhead
- Tight coupling despite abstraction

**Key Files**:
- `src/yars/util/Observable.h` - Base observable class
- `src/yars/util/Observer.h` - Observer interface
- `src/yars/util/ObservableMessage.h` - Message types
- `src/yars/main/YarsMainControl.cpp` - Central hub of Observer usage

### Critical Dependencies to Replace

| Component | Current | Target | Priority |
|-----------|---------|--------|----------|
| Filesystem | boost::filesystem | std::filesystem | HIGH |
| Threading | boost::thread | std::thread | HIGH |
| Program Options | boost::program_options | CLI11 | MEDIUM |
| Date/Time | boost::date_time | std::chrono | LOW |
| Configuration | Xerces-C++ XML | nlohmann/json | HIGH |

### Build Instructions
```bash
# Current build (may fail due to Ogre issues)
mkdir build && cd build
cmake ..
make -j8

# Test command
./bin/yars --iterations 1000 ../xml/braitenberg.xml
```

### Known Issues
1. **Ogre3D Integration**: Version 13.6.4 has API incompatibilities
2. **Observer Pattern**: Makes code flow nearly impossible to trace
3. **Factory Explosion**: 80+ factory classes for XML parsing
4. **Memory Management**: Raw pointers throughout
5. **Build System**: CMake configuration is complex and fragile

### Refactoring Execution Plan
See `REFACTORING_PLAN.md` for detailed step-by-step plan.

**Phase Overview**:
1. **Phase 1**: Remove Observer Pattern (Week 1-2)
2. **Phase 2**: C++ Modernization (Week 3-4)
3. **Phase 3**: Configuration Overhaul (Week 5-6)
4. **Phase 4**: Graphics Update (Week 7-8)
5. **Phase 5**: Testing & Polish (Week 9-10)

### Critical Success Criteria
- **Functionality**: All existing scenarios must work
- **Performance**: No regression, target 30% improvement
- **Memory**: Zero leaks, smart pointers throughout
- **Maintainability**: Clear, traceable execution flow

### Validation Commands
```bash
# Basic functionality test
./bin/yars --iterations 100 --no-gui ../xml/braitenberg.xml

# GUI test
./bin/yars --iterations 1000 ../xml/braitenberg.xml

# Performance test
time ./bin/yars --iterations 10000 --no-gui ../xml/falling_objects.xml

# Memory test
valgrind --leak-check=full ./bin/yars --iterations 100 ../xml/braitenberg.xml
```

### File Organization

#### Core Components
- `/src/yars/main/` - Main control and entry point
- `/src/yars/physics/bullet/` - Physics engine integration
- `/src/yars/view/gui/` - Graphics and visualization
- `/src/yars/configuration/` - XML parsing and configuration
- `/src/yars/util/` - Utilities including Observer pattern
- `/src/yars/logging/` - Data logging system

#### Key Entry Points
- `src/yars/yarsMain.cpp` - Main entry point
- `src/yars/main/YarsMainControl.cpp` - Central controller
- `src/yars/physics/YarsPhysicsControl.cpp` - Physics controller
- `src/yars/view/YarsViewControl.cpp` - View controller

### Refactoring Guidelines

#### When Removing Observer Pattern
1. Map all message types to direct method calls
2. Replace inheritance with composition
3. Use dependency injection for loose coupling
4. Implement callbacks only where async is needed

#### When Modernizing C++
1. Use smart pointers (`unique_ptr`, `shared_ptr`)
2. Prefer `std::` over Boost equivalents
3. Use auto where it improves readability
4. Apply RAII consistently

#### When Updating Configuration
1. Keep backward compatibility during transition
2. Validate all conversions
3. Simplify factory pattern usage
4. Document schema changes

### Testing Strategy
1. **Baseline**: Record current behavior before changes
2. **Unit Tests**: Test individual components
3. **Integration**: Test component interactions
4. **Regression**: Ensure no functionality loss
5. **Performance**: Benchmark against baseline

### Common Pitfalls to Avoid
1. **Circular Dependencies**: Plan changes to avoid cycles
2. **Big Bang Refactoring**: Make incremental changes
3. **Missing Validation**: Test after each change
4. **Lost Functionality**: Keep compatibility layers
5. **Performance Regression**: Profile regularly

### Progress Tracking
- GitHub Issues: Use template in `.github/ISSUE_TEMPLATE/`
- Refactoring Plan: `REFACTORING_PLAN.md`
- Analysis: `analysis.md`
- This file: Update with findings during refactoring

### Commands for Development
```bash
# Format code
clang-format -i src/yars/**/*.{h,cpp}

# Find Observer usage
grep -r "Observable\|Observer" src/ --include="*.cpp" --include="*.h"

# Count lines of code
find src/ -name "*.cpp" -o -name "*.h" | xargs wc -l

# Generate dependency graph
tools/analyze_dependencies.py > dependencies.dot
dot -Tpng dependencies.dot -o dependencies.png

# Profile performance
perf record ./bin/yars --iterations 1000 --no-gui ../xml/falling_objects.xml
perf report
```

### Critical Findings Log

#### 2024-12-XX: Initial Analysis
- Observer pattern is more pervasive than expected
- 30+ message types, 100+ notify calls
- Every major component inherits from Observable or Observer
- Message passing adds 10-15% performance overhead

#### [Add new findings here as refactoring progresses]

### Next Steps
1. Create development branch
2. Setup CI/CD pipeline
3. Create baseline tests
4. Begin Phase 1: Observer Removal

### Contact
For questions about this refactoring, consult:
- `REFACTORING_PLAN.md` - Detailed execution plan
- `analysis.md` - Initial codebase analysis
- GitHub Issues - Task tracking and progress