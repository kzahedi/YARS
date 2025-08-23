set(STOP False)

# includes
include(CMakeDetermineCXXCompiler)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Handle modern Boost CMake policy
cmake_policy(SET CMP0167 NEW)

# Modern Boost handling
if(APPLE)
  set(Boost_USE_STATIC_LIBS ON)
endif(APPLE)
find_package(Boost REQUIRED COMPONENTS program_options)
# Modern CMake: Use imported targets instead of include directories
# For header-only libraries like circular_buffer, we still need include directories
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

find_package(XercesC REQUIRED)
# Modern CMake: Use imported targets instead of manual includes
# Targets will link with XercesC::XercesC directly

if(YARS_USE_PYTHON)
find_package(PythonLibs 3 REQUIRED)
  IF(PYTHONLIBS_FOUND)
    include_directories(${PYTHON_INCLUDE_PATH})
  ENDIF(PYTHONLIBS_FOUND)
endif(YARS_USE_PYTHON)

# Modern CMake approach: let CMake find Bullet automatically
find_package(Bullet REQUIRED)
if(BULLET_FOUND)
  # Modern CMake: prefer target-based includes over directory includes
  # include_directories(${BULLET_INCLUDE_DIRS})
  # Note: Individual targets will link with Bullet::Bullet* targets
endif(BULLET_FOUND)

if(YARS_DOCS)
  find_package(LATEX)
endif(YARS_DOCS)

IF(YARS_USE_VISUALISATION)

  # Use pkg-config to find SDL2 properly
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(SDL2 REQUIRED sdl2)

  include_directories(${SDL2_INCLUDE_DIRS})

  # Create SDL2::SDL2 target that OGRE expects
  if(NOT TARGET SDL2::SDL2)
    add_library(SDL2::SDL2 INTERFACE IMPORTED)
    set_target_properties(SDL2::SDL2 PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIRS}"
      INTERFACE_LINK_LIBRARIES "${SDL2_LIBRARIES}")
  endif()

  # Find ZLIB for OGRE
  find_package(ZLIB REQUIRED)

  # Use local OGRE installation with modern CMake config
  set(OGRE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/ext/ogre/install)
  set(CMAKE_PREFIX_PATH ${OGRE_ROOT}/CMake ${CMAKE_PREFIX_PATH})
  find_package(OGRE REQUIRED CONFIG)

  # Add OGRE include directories (both parent and OGRE subdirectory)
  include_directories(${OGRE_ROOT}/include)
  include_directories(${OGRE_ROOT}/include/OGRE)

  # Set OGRE_FOUND to true for compatibility with existing checks
  set(OGRE_FOUND TRUE)

  if(UNIX AND NOT APPLE)
    add_definitions(-pthread)
    if(CMAKE_BUILD_TYPE MATCHES Release)
      add_definitions(-D_FORTIFY_SOURCE=2)
    endif(CMAKE_BUILD_TYPE MATCHES Release)
  endif(UNIX AND NOT APPLE)
ENDIF(YARS_USE_VISUALISATION)

if(YARS_TESTCASES)
  find_package(UnitTest++)
  IF(UNITTEST++_FOUND)
    include_directories(${UNITTEST++_INCLUDE_DIR})
    link_directories(${UNITTEST++_LIBRARY_DIR})
  ENDIF(UNITTEST++_FOUND)
endif(YARS_TESTCASES)


if(NOT Boost_FOUND)
  set(stop TRUE)
endif(NOT Boost_FOUND)

if(NOT XERCESC_FOUND)
  set(stop TRUE)
endif(NOT XERCESC_FOUND)

IF(YARS_USE_VISUALISATION)
  if(NOT SDL2_FOUND)
    set(stop TRUE)
  endif(NOT SDL2_FOUND)
  if(NOT OGRE_FOUND)
    set(stop TRUE)
    MESSAGE("\nOGRE not found")
  endif(NOT OGRE_FOUND)
ENDIF(YARS_USE_VISUALISATION)

if(NOT BULLET_FOUND)
  set(stop TRUE)
endif(NOT BULLET_FOUND)
