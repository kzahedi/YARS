# YARS Dependencies Upgrade Guide

## Current Dependencies Analysis

### Core Dependencies
| Library | Current Version | Usage | Status |
|---------|----------------|-------|--------|
| Boost | 1.67+ | Filesystem, Thread, Program Options, Date/Time | Partially deprecated |
| Xerces-C++ | 3.x | XML parsing and validation | To be replaced |
| Bullet Physics | 3.x | Physics simulation | Keep current |
| Ogre3D | 13.6.4 | 3D rendering | Needs fixing or replacement |
| SDL2 | 2.x | Window management | Keep current |
| Python | 3.x | Controller scripting | Keep current |
| Qt5 | 5.x | Client GUI | Optional, keep |

## Upgrade Paths

### 1. Boost → Standard Library

#### Boost::filesystem → std::filesystem (C++17)

**Current Usage**:
```cpp
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
fs::path path = fs::initial_path();
if (fs::exists(path)) { ... }
```

**Migration**:
```cpp
#include <filesystem>
namespace fs = std::filesystem;
fs::path path = fs::current_path();
if (fs::exists(path)) { ... }
```

**Files Affected**:
- `src/yars/util/FileSystemOperations.cpp`
- `src/yars/util/Directories.cpp`
- `src/yars/configuration/YarsConfiguration.cpp`

**Migration Steps**:
1. Replace includes
2. Update namespace alias
3. Change `initial_path()` to `current_path()`
4. Update exception handling
5. Test file operations

#### Boost::thread → std::thread (C++11)

**Current Usage**:
```cpp
#include <boost/thread.hpp>
boost::thread t(&function);
boost::mutex m;
boost::unique_lock<boost::mutex> lock(m);
```

**Migration**:
```cpp
#include <thread>
#include <mutex>
std::thread t(&function);
std::mutex m;
std::unique_lock<std::mutex> lock(m);
```

**Files Affected**:
- `src/yars/main/MainLoopThread.cpp`
- `src/yars/view/gui/GuiMutex.cpp`

#### Boost::program_options → CLI11

**Current Usage**:
```cpp
#include <boost/program_options.hpp>
namespace po = boost::program_options;
po::options_description desc("Options");
desc.add_options()
    ("help", "Show help")
    ("iterations", po::value<int>(), "Number of iterations");
```

**Migration**:
```cpp
#include <CLI/CLI.hpp>
CLI::App app{"YARS - Yet Another Robot Simulator"};
app.add_flag("-h,--help", "Show help");
app.add_option("-i,--iterations", iterations, "Number of iterations");
```

**Installation**:
```bash
# Add to CMakeLists.txt
include(FetchContent)
FetchContent_Declare(
    CLI11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_TAG v2.3.2
)
FetchContent_MakeAvailable(CLI11)
target_link_libraries(yars CLI11::CLI11)
```

#### Boost::date_time → std::chrono (C++11)

**Current Usage**:
```cpp
#include <boost/date_time/posix_time/posix_time.hpp>
boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
```

**Migration**:
```cpp
#include <chrono>
auto now = std::chrono::system_clock::now();
```

### 2. Xerces-C++ → nlohmann/json

**Current XML Configuration**:
```xml
<robot name="vehicle">
  <position x="0" y="0" z="1"/>
  <sensor type="proximity" range="10"/>
</robot>
```

**New JSON Configuration**:
```json
{
  "robot": {
    "name": "vehicle",
    "position": {"x": 0, "y": 0, "z": 1},
    "sensor": {"type": "proximity", "range": 10}
  }
}
```

**Installation**:
```bash
# Add to CMakeLists.txt
include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)
target_link_libraries(yars nlohmann_json::nlohmann_json)
```

**Migration Code**:
```cpp
// Old Xerces parsing
#include <xercesc/parsers/XercesDOMParser.hpp>
XercesDOMParser* parser = new XercesDOMParser();
parser->parse(xmlFile);
DOMDocument* doc = parser->getDocument();

// New JSON parsing
#include <nlohmann/json.hpp>
#include <fstream>
std::ifstream file(jsonFile);
nlohmann::json config = nlohmann::json::parse(file);
auto robotName = config["robot"]["name"].get<std::string>();
```

### 3. Ogre3D Options

#### Option A: Fix Ogre3D 13.6.4 Integration

**Known Issues**:
- API changes in Camera positioning
- SceneManager creation changes
- RTShaderSystem initialization required

**Fix Steps**:
1. Add Ogre as git submodule
2. Build as part of YARS
3. Update API calls
4. Initialize RTSS

#### Option B: Upgrade to Ogre3D 14.x

**Changes Required**:
- Update CMake find modules
- Update material scripts
- Port to new shader system
- Update overlay system

#### Option C: Modern OpenGL

**Dependencies**:
```bash
# GLFW for windowing
# GLM for mathematics
# GLAD for OpenGL loading
# stb_image for texture loading
```

**CMake Integration**:
```cmake
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)

add_library(glad src/glad.c)
target_include_directories(glad PUBLIC include)

target_link_libraries(yars glfw glm glad)
```

### 4. Build System Updates

#### CMake Modernization

**Current** (CMake 3.0):
```cmake
cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
```

**Modern** (CMake 3.20+):
```cmake
cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use FetchContent for dependencies
include(FetchContent)

# Use target-based commands
target_compile_features(yars PUBLIC cxx_std_20)
target_include_directories(yars PUBLIC include)
```

### 5. Testing Framework

#### Add Google Test

**Installation**:
```cmake
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(yars_tests tests/main.cpp)
target_link_libraries(yars_tests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(yars_tests)
```

### 6. Package Management Options

#### Option A: vcpkg
```bash
# Install vcpkg
git clone https://github.com/Microsoft/vcpkg.git
./vcpkg/bootstrap-vcpkg.sh

# Install dependencies
./vcpkg install bullet3 sdl2 nlohmann-json cli11 gtest

# Configure CMake
cmake -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake ..
```

#### Option B: Conan
```bash
# Install Conan
pip install conan

# Create conanfile.txt
[requires]
bullet3/3.24
sdl2/2.28.5
nlohmann_json/3.11.2
cli11/2.3.2
gtest/1.14.0

[generators]
CMakeDeps
CMakeToolchain

# Install dependencies
conan install . --build=missing
```

#### Option C: FetchContent (Recommended)
```cmake
# All dependencies fetched at configure time
# No external package manager needed
# Reproducible builds
# Version controlled
```

## Migration Strategy

### Phase 1: Remove Boost (Week 1)
1. Start with boost::filesystem → std::filesystem
2. Then boost::thread → std::thread
3. Install CLI11 for program options
4. Finally boost::date_time → std::chrono

### Phase 2: Configuration System (Week 2)
1. Design JSON schema
2. Install nlohmann/json
3. Create compatibility layer
4. Migrate configurations

### Phase 3: Graphics (Week 3)
1. Evaluate Ogre alternatives
2. Implement chosen solution
3. Port rendering code
4. Update materials/shaders

### Phase 4: Testing (Week 4)
1. Install Google Test
2. Create test structure
3. Write unit tests
4. Setup CI/CD

## Validation Steps

### After Each Dependency Change
1. **Compilation**: `make clean && make`
2. **Link Check**: `ldd bin/yars | grep <old_lib>` should return nothing
3. **Functionality**: Run test scenarios
4. **Performance**: Compare with baseline

### Final Validation
```bash
# Check for old dependencies
ldd bin/yars | grep -E "boost|xerces"
# Should return nothing

# Verify new dependencies
ldd bin/yars | grep -E "stdc++fs|pthread"
# Should show standard library versions

# Run full test suite
ctest --verbose

# Memory check
valgrind --leak-check=full ./bin/yars
```

## Rollback Procedures

### For Each Dependency
1. Keep old code in `#ifdef OLD_DEPS` blocks initially
2. Create feature flags for gradual migration
3. Maintain compatibility layer temporarily
4. Remove old code only after validation

### Emergency Rollback
```bash
git checkout main
git branch -D refactoring/dependencies
cmake -DUSE_OLD_DEPS=ON ..
make
```

## Common Issues and Solutions

### Issue: std::filesystem not found
**Solution**: Ensure C++17 or later, link with `-lstdc++fs` on older GCC

### Issue: CLI11 parsing differences
**Solution**: Create wrapper to match old interface initially

### Issue: JSON performance
**Solution**: Use SAX parser for large files, cache parsed data

### Issue: Graphics compatibility
**Solution**: Abstract renderer interface, support multiple backends

## Performance Considerations

### Expected Improvements
- **Boost removal**: 5-10% compile time reduction
- **JSON vs XML**: 2-3x faster parsing
- **Modern C++**: 10-15% runtime improvement
- **Smart pointers**: Eliminate memory leaks

### Potential Regressions
- **JSON**: Larger file sizes (mitigate with compression)
- **std::filesystem**: Slightly slower on some operations
- **Graphics change**: Initial performance tuning needed

## Documentation Updates

### Files to Update
1. `README.md` - New dependencies and build instructions
2. `INSTALL.md` - Installation guide
3. `docs/configuration.md` - New JSON format
4. `docs/api.md` - API changes
5. `MIGRATION.md` - User migration guide

### User Communication
1. Announce deprecation schedule
2. Provide migration tools
3. Maintain compatibility period
4. Document breaking changes
5. Provide support timeline