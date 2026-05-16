# Modern CMake version for best practices
cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

project(yars
  VERSION 0.8.8
  DESCRIPTION "YARS - Yet Another Robot Simulator"
  LANGUAGES CXX)
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

# Default to an optimized build. A bare `cmake ..` in a fresh build dir
# would otherwise leave CMAKE_BUILD_TYPE empty and produce an unoptimized
# binary with no -O flags — the simulator ran ~40% slower until we
# noticed. Multi-config generators (Xcode, Visual Studio) ignore this.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Build type (Release, Debug, RelWithDebInfo, MinSizeRel)" FORCE)
endif()

# Version information automatically set by project() command above.
# Propagate to C++ so `yars --version` can print the binary version
# alongside the XML schema version that XmlChangeLog tracks.
add_compile_definitions(YARS_VERSION_STRING="${PROJECT_VERSION}")

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

# Modern Google Test framework
option(YARS_BUILD_TESTS "Build YARS tests" ON)
if(YARS_BUILD_TESTS)
  add_subdirectory(tests)
endif()

IF(YARS_TESTCASES)
  ENABLE_TESTING()
  add_subdirectory(testCases)
ENDIF(YARS_TESTCASES)

INCLUDE(cmake/InstallConfiguration.cmake)
INCLUDE(cmake/CPackConfiguration.cmake)
INCLUDE(cmake/UninstallConfiguration.cmake)
INCLUDE(cmake/CreateSymbolicLinks.cmake)

  # Test executables for debugging OGRE issues (if visualization is enabled) - DISABLED
if(FALSE)
  # Step-by-step OGRE test
  # add_executable(test_step_by_step test_step_by_step.cpp)
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
      freetype zzip z
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
      freetype zzip z
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
      freetype zzip z
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
endif(FALSE)
