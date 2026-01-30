# CMake minimum version must come first
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(yars)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CMake policies
cmake_policy(SET CMP0005 NEW)
cmake_policy(SET CMP0003 NEW)
cmake_policy(SET CMP0007 NEW) # list does not count empty elements
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0054 NEW)

set(LIBRARY_TYPE STATIC)

# Set Version Information
# TODO: read this from defines/version.h
set(YARS_MAJOR 0)
set(YARS_MINOR 7)
set(YARS_PATCH 28)

INCLUDE(cmake/SetOptions.cmake)
INCLUDE(cmake/IncludePackages.cmake) # must be called first
INCLUDE(cmake/SetDirectories.cmake)
INCLUDE(cmake/SetCompilerSettings.cmake)
INCLUDE(cmake/SetDefinitions.cmake)
INCLUDE(cmake/PrintConfiguration.cmake)
INCLUDE(cmake/CreateConfigFiles.cmake)

# if(YARS_DOXYGEN_DOCUMENTATION)
  # add_subdirectory(doxygen)
# endif(YARS_DOXYGEN_DOCUMENTATION)
add_subdirectory(src/yars)
add_subdirectory(contrib)

IF(YARS_TESTCASES)
  ENABLE_TESTING()
  add_subdirectory(testCases)
ENDIF(YARS_TESTCASES)

INCLUDE(cmake/InstallConfiguration.cmake)
INCLUDE(cmake/CPackConfiguration.cmake)
INCLUDE(cmake/UninstallConfiguration.cmake)
INCLUDE(cmake/CreateSymbolicLinks.cmake)
