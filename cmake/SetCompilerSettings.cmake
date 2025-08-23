set(CMAKE_VERBOSE_MAKEFILE OFF)

# Modern CMake: Use standard properties instead of manual compiler settings
# Let CMake choose the best compiler automatically
# set(CMAKE_BASE_NAME              "g++")
# set(CMAKE_CXX_COMPILER           "g++")
# set(CMAKE_CXX_COMPILER_FULLPATH  "g++")

# Modern CMake: Use standard properties for output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Modern CMake: Set C++ standard globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Modern CMake: Use compile features and properties instead of manual flags
# Enable modern warnings and debugging
set(CMAKE_CXX_FLAGS_DEBUG   "-g -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Position Independent Code (PIC) is handled automatically by modern CMake
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Platform-specific optimizations (modern approach)
if(APPLE)
  # macOS specific optimizations
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
elseif(UNIX)
  # Linux specific optimizations  
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto")
endif()

# Modern CMake: Use generator expressions for conditional compilation
# This replaces the old manual flag setting approach


