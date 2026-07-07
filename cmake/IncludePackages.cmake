set(STOP False)

# includes
include(CMakeDetermineCXXCompiler)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# CLI11 for command-line parsing (replaces boost::program_options)
find_package(CLI11 CONFIG REQUIRED)

find_package(XercesC REQUIRED)
# Modern CMake: Use imported targets instead of manual includes
# Targets will link with XercesC::XercesC directly

# nlohmann/json (vendored single header, v3.11.3) — used by the JSON
# config-migration Stage 0 converter (src/yars/configuration/json/).
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/ext/json/include")

if(YARS_USE_PYTHON)
find_package(PythonLibs 3 REQUIRED)
  IF(PYTHONLIBS_FOUND)
    include_directories(${PYTHON_INCLUDE_PATH})
  ENDIF(PYTHONLIBS_FOUND)
endif(YARS_USE_PYTHON)

# Prefer the source-built SIMD Bullet (ext/bullet/install) when present;
# fall back to the system package otherwise. Mirrors the Ogre pattern.
if(EXISTS "${CMAKE_SOURCE_DIR}/ext/bullet/install")
  set(BULLET_ROOT "${CMAKE_SOURCE_DIR}/ext/bullet/install")
  list(PREPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/ext/bullet/install")
endif()

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

  # Create SDL2::SDL2 target that OGRE expects.
  # On macOS, brew installs SDL2 under /opt/homebrew/lib which is not on
  # the default linker search path; SDL2_LIBRARY_DIRS (from pkg-config) is
  # required for the linker to find -lSDL2. On Linux the dir is already
  # in the default search path but setting it does no harm.
  if(NOT TARGET SDL2::SDL2)
    add_library(SDL2::SDL2 INTERFACE IMPORTED)
    set_target_properties(SDL2::SDL2 PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${SDL2_INCLUDE_DIRS}"
      INTERFACE_LINK_LIBRARIES "${SDL2_LIBRARIES}"
      INTERFACE_LINK_DIRECTORIES "${SDL2_LIBRARY_DIRS}")
  endif()

  # Find ZLIB for OGRE
  find_package(ZLIB REQUIRED)

  # Use local OGRE installation with modern CMake config.
  set(OGRE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/ext/ogre/install)
  set(CMAKE_PREFIX_PATH ${OGRE_ROOT}/CMake ${CMAKE_PREFIX_PATH})

  # macOS-only: Ogre 14 dynamic install puts plugins/frameworks under
  # lib/macosx/Release/, but Ogre's exported OGREConfig.cmake hard-codes
  # OGRE_PLUGIN_DIR = ${OGRE_ROOT}/lib/OGRE (Linux layout) and aborts
  # find_package() if that path doesn't exist. Create a symlink so the
  # find_package check passes; plugins.cfg's PluginFolder=${OGRE_ROOT}/lib/OGRE/
  # also resolves through the same symlink at runtime.
  if(APPLE AND NOT EXISTS "${OGRE_ROOT}/lib/OGRE")
    if(EXISTS "${OGRE_ROOT}/lib/macosx/Release")
      execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink macosx/Release "${OGRE_ROOT}/lib/OGRE")
    endif()
  endif()

  # Try Ogre's exported CMake config first; it transparently handles the
  # platform-specific install layout (.so on Linux, .dylib/.framework on
  # macOS) and exports imported targets like OgreMain, OgreOverlay,
  # OgreRTShaderSystem. Falls back to manual include paths if config
  # isn't installed (e.g. partial installs).
  find_package(OGRE CONFIG QUIET COMPONENTS Overlay RTShaderSystem)
  # Always add both include roots: YARS source uses <OGRE/...> form, and
  # Ogre's internal headers do bare-name #include "OgreRenderable.h" et al.
  # Imported targets give us link info but not enough transitive includes.
  include_directories(${OGRE_ROOT}/include)
  include_directories(${OGRE_ROOT}/include/OGRE)
  if(NOT OGRE_FOUND)
    # Manual-path fallback for environments where the exported config
    # isn't installed.
    set(OGRE_FOUND TRUE)
  endif()

  # Locate Ogre's installed Media directory (ships spot_shadow_fade.dds
  # etc.). The path is platform-dependent: macOS installs to
  # ${prefix}/Media/Main, Linux to ${prefix}/share/OGRE-<SOVERSION>/Media/Main.
  # Probe both so OgreHandler can reference it via the YARS_OGRE_MEDIA_DIR
  # compile-time define.
  set(_yars_ogre_media_candidates
    "${OGRE_ROOT}/Media"
    "${OGRE_ROOT}/share/OGRE-14.4/Media"
    "${OGRE_ROOT}/share/OGRE-14.3/Media"
    "${OGRE_ROOT}/share/OGRE/Media")
  set(YARS_OGRE_MEDIA_DIR "")
  foreach(_yars_candidate ${_yars_ogre_media_candidates})
    if(EXISTS "${_yars_candidate}/Main/spot_shadow_fade.dds")
      set(YARS_OGRE_MEDIA_DIR "${_yars_candidate}")
      break()
    endif()
  endforeach()
  if(YARS_OGRE_MEDIA_DIR)
    message(STATUS "Ogre Media located at ${YARS_OGRE_MEDIA_DIR}")
    add_compile_definitions(YARS_OGRE_MEDIA_DIR="${YARS_OGRE_MEDIA_DIR}")
  else()
    message(WARNING "Ogre Media directory not found under ${OGRE_ROOT}; "
                    "shadows requiring spot_shadow_fade.dds will fail to load")
    add_compile_definitions(YARS_OGRE_MEDIA_DIR="${OGRE_ROOT}/Media")
  endif()

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
