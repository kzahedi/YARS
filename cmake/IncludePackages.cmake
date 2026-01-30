set(STOP False)

# includes
include(CMakeDetermineCXXCompiler)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Handle modern Boost (1.70+) which uses BoostConfig.cmake
# system and thread are header-only in modern Boost
if(APPLE)
  set(Boost_USE_STATIC_LIBS ON)
endif(APPLE)

# Boost components needed (thread requires linking on some platforms)
find_package(Boost REQUIRED COMPONENTS program_options filesystem thread)
IF(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
ENDIF(Boost_FOUND)

find_package(XercesC)
IF(XERCESC_FOUND)
  include_directories(${XERCESC_INCLUDE_DIR})
ENDIF(XERCESC_FOUND)

if(YARS_USE_PYTHON)
find_package(PythonLibs 3 REQUIRED)
  IF(PYTHONLIBS_FOUND)
    include_directories(${PYTHON_INCLUDE_PATH})
  ENDIF(PYTHONLIBS_FOUND)
endif(YARS_USE_PYTHON)

find_package(Bullet)
if(BULLET_FOUND)
  include_directories(${BULLET_INCLUDE_DIR})
endif(BULLET_FOUND)

if(YARS_DOCS)
  find_package(LATEX)
endif(YARS_DOCS)

IF(YARS_USE_VISUALISATION)

  find_package(SDL2)
  include_directories(${SDL2_INCLUDE_DIR})

  # ===========================================================
  # LOCAL OGRE BUILD (ext/ogre)
  # ===========================================================
  # Use the OGRE we built in ext/ogre/build
  set(OGRE_BUILD_DIR "${CMAKE_SOURCE_DIR}/ext/ogre/build")
  set(OGRE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/ext/ogre")

  if(APPLE)
    # On macOS, OGRE builds as frameworks
    set(OGRE_FRAMEWORK_DIR "${OGRE_BUILD_DIR}/lib/macosx")

    # Verify OGRE was built
    if(NOT EXISTS "${OGRE_FRAMEWORK_DIR}/Ogre.framework")
      message(FATAL_ERROR "OGRE not built. Please run: cd ext/ogre/build && cmake .. && make")
    endif()

    # Set framework paths
    set(CMAKE_FRAMEWORK_PATH ${CMAKE_FRAMEWORK_PATH} ${OGRE_FRAMEWORK_DIR})

    # Include directories - use wrapper structure for <OGRE/Ogre.h> style includes
    # The ${OGRE_BUILD_DIR}/include/OGRE directory has symlinks to source headers
    set(OGRE_INCLUDE_DIRS
      "${OGRE_BUILD_DIR}/include"           # For <OGRE/Ogre.h> style includes
      "${OGRE_BUILD_DIR}/include/OGRE"      # For Overlay headers that need main OGRE headers without prefix
      "${OGRE_FRAMEWORK_DIR}/Ogre.framework/Headers"  # Generated headers
    )

    # Libraries (frameworks)
    set(OGRE_LIBRARIES
      "-framework Ogre"
      "-framework OgreOverlay"
      "-framework OgreRTShaderSystem"
      "-framework RenderSystem_GL3Plus"
      "-framework Plugin_ParticleFX"
      "-framework Codec_STBI"
      "-framework OgreBites"
      "-framework Cocoa"
      "-framework OpenGL"
      "-framework IOKit"
      "-framework CoreFoundation"
      "-framework CoreGraphics"
    )

    # Link directory for frameworks
    link_directories(${OGRE_FRAMEWORK_DIR})

    set(OGRE_FOUND TRUE)
    set(OGRE_Overlay_LIBRARIES "-framework OgreOverlay")

    message(STATUS "Using local OGRE build from: ${OGRE_FRAMEWORK_DIR}")

  else()
    # Linux: Not implemented yet
    message(FATAL_ERROR "Linux OGRE integration not yet implemented")
  endif()

  include_directories(${OGRE_INCLUDE_DIRS})
  # ===========================================================

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
