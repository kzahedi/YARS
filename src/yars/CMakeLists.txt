add_executable(${YARS_TARGET} yarsMain.cpp)

# Add Bullet Physics headers needed by yarsMain.cpp
target_include_directories(${YARS_TARGET} PRIVATE ${BULLET_INCLUDE_DIRS})

add_subdirectory(types)
add_subdirectory(configuration)
add_subdirectory(view/console)
add_subdirectory(physics)
add_subdirectory(util)
add_subdirectory(main)
add_subdirectory(logging)
add_subdirectory(rc)

target_link_libraries(${YARS_TARGET} m stdc++ pthread dl)
target_link_libraries(${YARS_TARGET} YarsMain)
target_link_libraries(${YARS_TARGET} YarsConfiguration)
target_link_libraries(${YARS_TARGET} YarsPhysics)
target_link_libraries(${YARS_TARGET} YarsUtil)
target_link_libraries(${YARS_TARGET} YarsLogging)
target_link_libraries(${YARS_TARGET} YarsViewConsole)
target_link_libraries(${YARS_TARGET} YarsDataTypes)

if(APPLE)
  target_link_libraries(${YARS_TARGET} objc)
endif(APPLE)

if(YARS_USE_CAPTURE_VIDEO)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(FFMPEG REQUIRED libavformat libavcodec libavutil libavfilter libswscale libavdevice libswresample)
  target_include_directories(${YARS_TARGET} PRIVATE ${FFMPEG_INCLUDE_DIRS})
  target_link_directories(${YARS_TARGET} PRIVATE ${FFMPEG_LIBRARY_DIRS})
  target_link_libraries(${YARS_TARGET} ${FFMPEG_LIBRARIES})
endif(YARS_USE_CAPTURE_VIDEO)

if(YARS_USE_VISUALISATION)

  add_subdirectory(view)
  add_subdirectory(view/gui)

  # Ogre 14 link. Prefer the imported targets exported by Ogre's CMake
  # config (`find_package(OGRE CONFIG)` in cmake/IncludePackages.cmake) so
  # the framework-vs-dylib layout on macOS is handled transparently.
  # If Ogre's config wasn't found, fall back to the Linux dynamic-lib path.
  if(TARGET OgreMain)
    target_link_libraries(${YARS_TARGET} OgreMain OgreOverlay OgreRTShaderSystem z)
  else()
    target_link_libraries(${YARS_TARGET}
      ${OGRE_ROOT}/lib/libOgreMain${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${OGRE_ROOT}/lib/libOgreOverlay${CMAKE_SHARED_LIBRARY_SUFFIX}
      ${OGRE_ROOT}/lib/libOgreRTShaderSystem${CMAKE_SHARED_LIBRARY_SUFFIX}
      z)
  endif()

  if(APPLE)
    # macOS-specific platform frameworks (windowing, GL, I/O kit) — SDL2
    # initialises Cocoa indirectly but Ogre's GL3+ context creation also
    # needs CoreVideo and IOKit.
    set_target_properties(${YARS_TARGET} PROPERTIES
      LINK_FLAGS "-framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")
  else()
    # X11 / GL / freetype etc. via the standard distro packages.
    target_link_libraries(${YARS_TARGET} X11 GL Xt zzip freetype)
  endif()

  target_link_libraries(${YARS_TARGET} SDL2::SDL2)
  target_link_libraries(${YARS_TARGET} YarsView)
  target_link_libraries(${YARS_TARGET} YarsGUI)
endif(YARS_USE_VISUALISATION)

option(YARS_USE_VULKAN "Build Vulkan renderer and tests" OFF)
if(YARS_USE_VULKAN)
  add_subdirectory(view/vulkan)
endif(YARS_USE_VULKAN)

INSTALL(TARGETS ${YARS_TARGET} RUNTIME DESTINATION bin CONFIGURATIONS Release Debug)