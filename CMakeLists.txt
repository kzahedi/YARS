project(yars)

# check for cmake version first:
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0005 NEW)
  cmake_policy(SET CMP0003 NEW)
  cmake_policy(SET CMP0006 NEW)
  cmake_policy(SET CMP0007 NEW) # list does not count empty elements
  cmake_policy(SET CMP0002 NEW)
  cmake_policy(SET CMP0042 NEW)
  cmake_policy(SET CMP0054 NEW)
  cmake_policy(SET CMP0009 NEW) # don't follow symlink for file(GLOB_RECURSE
  cmake_policy(SET CMP0045 NEW) # don't follow symlink for file(GLOB_RECURSE
endif(COMMAND cmake_policy)

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

  # Test executables for debugging OGRE issues (if visualization is enabled)
if(YARS_USE_VISUALISATION)
  # Step-by-step OGRE test
  add_executable(test_step_by_step test_step_by_step.cpp)
  target_link_libraries(test_step_by_step
    YarsConfiguration
    YarsGUI
    YarsView
    YarsDataTypes
    YarsUtil
    YarsMain
    YarsLogging
    YarsViewConsole
    ${Boost_LIBRARIES}
    boost_program_options
    ${XERCESC_LIBRARY_FILE}
    m stdc++ pthread dl
  )

  # Minimal OGRE test
  add_executable(test_minimal_ogre test_minimal_ogre.cpp)
  target_link_libraries(test_minimal_ogre
    YarsConfiguration
    YarsGUI
    YarsView
    YarsDataTypes
    YarsUtil
    YarsMain
    YarsLogging
    YarsViewConsole
    ${Boost_LIBRARIES}
    boost_program_options
    ${XERCESC_LIBRARY_FILE}
    m stdc++ pthread dl
  )

  # Native window test
  add_executable(test_native_window test_native_window.cpp)
  target_link_libraries(test_native_window
    YarsConfiguration
    YarsGUI
    YarsView
    YarsDataTypes
    YarsUtil
    YarsMain
    YarsLogging
    YarsViewConsole
    ${Boost_LIBRARIES}
    boost_program_options
    ${XERCESC_LIBRARY_FILE}
    m stdc++ pthread dl
  )

  if(APPLE)
    # Use same OGRE linking as main yars executable
    set(OGRE_ROOT ${CMAKE_SOURCE_DIR}/ext/ogre/install)
    target_link_libraries(test_step_by_step
      freeimage freetype zzip z
      ${OGRE_ROOT}/lib/libOgreMainStatic.a
      ${OGRE_ROOT}/lib/libOgreOverlayStatic.a
      ${OGRE_ROOT}/lib/libOgreRTShaderSystemStatic.a
      ${OGRE_ROOT}/lib/libOgreGLSupportStatic.a
      ${OGRE_ROOT}/lib/OGRE/libRenderSystem_GL3PlusStatic.a
      ${OGRE_ROOT}/lib/OGRE/libPlugin_ParticleFXStatic.a
      ${OGRE_ROOT}/lib/OGRE/libPlugin_OctreeSceneManagerStatic.a
      ${OGRE_ROOT}/lib/OGRE/libCodec_STBIStatic.a
      ${OGRE_ROOT}/lib/OGRE/libCodec_FreeImageStatic.a
      SDL2
      objc
    )
    SET_TARGET_PROPERTIES(test_step_by_step PROPERTIES LINK_FLAGS "-framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")

    target_link_libraries(test_minimal_ogre
      freeimage freetype zzip z
      ${OGRE_ROOT}/lib/libOgreMainStatic.a
      ${OGRE_ROOT}/lib/libOgreOverlayStatic.a
      ${OGRE_ROOT}/lib/libOgreRTShaderSystemStatic.a
      ${OGRE_ROOT}/lib/libOgreGLSupportStatic.a
      ${OGRE_ROOT}/lib/OGRE/libRenderSystem_GL3PlusStatic.a
      ${OGRE_ROOT}/lib/OGRE/libPlugin_ParticleFXStatic.a
      ${OGRE_ROOT}/lib/OGRE/libPlugin_OctreeSceneManagerStatic.a
      ${OGRE_ROOT}/lib/OGRE/libCodec_STBIStatic.a
      ${OGRE_ROOT}/lib/OGRE/libCodec_FreeImageStatic.a
      SDL2
      objc
    )
    SET_TARGET_PROPERTIES(test_minimal_ogre PROPERTIES LINK_FLAGS "-framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")

    target_link_libraries(test_native_window
      freeimage freetype zzip z
      ${OGRE_ROOT}/lib/libOgreMainStatic.a
      ${OGRE_ROOT}/lib/libOgreOverlayStatic.a
      ${OGRE_ROOT}/lib/libOgreRTShaderSystemStatic.a
      ${OGRE_ROOT}/lib/libOgreGLSupportStatic.a
      ${OGRE_ROOT}/lib/OGRE/libRenderSystem_GL3PlusStatic.a
      ${OGRE_ROOT}/lib/OGRE/libPlugin_ParticleFXStatic.a
      ${OGRE_ROOT}/lib/OGRE/libPlugin_OctreeSceneManagerStatic.a
      ${OGRE_ROOT}/lib/OGRE/libCodec_STBIStatic.a
      ${OGRE_ROOT}/lib/OGRE/libCodec_FreeImageStatic.a
      SDL2
      objc
    )
    SET_TARGET_PROPERTIES(test_native_window PROPERTIES LINK_FLAGS "-framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")
  endif(APPLE)
endif(YARS_USE_VISUALISATION)
