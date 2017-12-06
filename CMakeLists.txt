project(yars)

# check for cmake version first:
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0005 OLD)
  cmake_policy(SET CMP0003 NEW)
  cmake_policy(SET CMP0006 OLD)
  cmake_policy(SET CMP0007 OLD) # list does not count empty elements
  cmake_policy(SET CMP0002 OLD)
  cmake_policy(SET CMP0042 OLD)
  cmake_policy(SET CMP0009 NEW) # don't follow symlink for file(GLOB_RECURSE
  cmake_policy(SET CMP0045 NEW) # don't follow symlink for file(GLOB_RECURSE
endif(COMMAND cmake_policy)

set(LIBRARY_TYPE STATIC)

# Set Version Information
# TODO: read this from defines/version.h
set(YARS_MAJOR 0)
set(YARS_MINOR 7)
set(YARS_PATCH 28)

INCLUDE(cmake/IncludePackages.cmake) # must be called first
INCLUDE(cmake/SetOptions.cmake)
INCLUDE(cmake/SetDirectories.cmake)
INCLUDE(cmake/SetCompilerSettings.cmake)
INCLUDE(cmake/SetDefinitions.cmake)
INCLUDE(cmake/PrintConfiguration.cmake)
INCLUDE(cmake/CreateConfigFiles.cmake)

if(YARS_DOXYGEN_DOCUMENTATION)
  add_subdirectory(doxygen)
endif(YARS_DOXYGEN_DOCUMENTATION)
add_subdirectory(src/yars)
add_subdirectory(contrib)

IF(YARS_TESTCASES)
  ENABLE_TESTING()
  add_subdirectory(testCases)
ENDIF(YARS_TESTCASES)

IF(APPLE)
  INCLUDE(cmake/InstallConfiguration.cmake)
ELSE(APPLE)
  IF(UNIX)
    INSTALL(DIRECTORY ${PROJECT_BINARY_DIR}/lib
            CONFIGURATIONS Release Debug
            DESTINATION ${CMAKE_INSTALL_PREFIX}
            FILES_MATCHING PATTERN "libYarsController*"
            )

    INSTALL(DIRECTORY ${PROJECT_BINARY_DIR}/lib
            CONFIGURATIONS Release Debug
            DESTINATION ${CMAKE_INSTALL_PREFIX}
            FILES_MATCHING PATTERN "libYarsClientCom*"
            )

    INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/src/
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include
            CONFIGURATIONS Release Debug
            PATTERN "*.cpp" EXCLUDE
            PATTERN "*.txt" EXCLUDE
            )

    INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/contrib/
            DESTINATION ${CMAKE_INSTALL_PREFIX}/include/yars/
            CONFIGURATIONS Release Debug
            PATTERN "*.cpp" EXCLUDE
            PATTERN "*.txt" EXCLUDE
            )
  ENDIF(UNIX)
ENDIF(APPLE)
INCLUDE(cmake/CPackConfiguration.cmake)
INCLUDE(cmake/UninstallConfiguration.cmake)
INCLUDE(cmake/CreateSymbolicLinks.cmake)
