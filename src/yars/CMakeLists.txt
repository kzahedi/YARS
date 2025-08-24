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

target_link_libraries(${YARS_TARGET} ${XERCESC_LIBRARY_FILE} ${Boost_LIBRARIES})

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
  target_link_libraries(${YARS_TARGET} avformat avcodec avutil avfilter avresample postproc
    swscale avdevice swresample)
endif(YARS_USE_CAPTURE_VIDEO)

if(YARS_USE_VISUALISATION)

  # Add codec framework headers for image loading support
  if(APPLE)
    set(OGRE_ROOT ${CMAKE_SOURCE_DIR}/ext/ogre/install)
    include_directories(${OGRE_ROOT}/lib/Codec_STBIStatic.framework/Headers)
    include_directories(${OGRE_ROOT}/lib/Codec_FreeImageStatic.framework/Headers)
  endif(APPLE)

  add_subdirectory(view)
  add_subdirectory(view/gui)

  if(APPLE)
    # Use explicit OGRE library paths for static linking
    set(OGRE_ROOT ${CMAKE_SOURCE_DIR}/ext/ogre/install)
    target_link_libraries(${YARS_TARGET}
      /opt/homebrew/lib/libfreetype.dylib
      ${OGRE_ROOT}/lib/libOgreMainStatic.a
      ${OGRE_ROOT}/lib/libOgreOverlayStatic.a
      ${OGRE_ROOT}/lib/libOgreRTShaderSystemStatic.a
      ${OGRE_ROOT}/lib/libOgreGLSupportStatic.a
      ${OGRE_ROOT}/lib/OGRE/libRenderSystem_GL3PlusStatic.a
      ${OGRE_ROOT}/lib/OGRE/libPlugin_ParticleFXStatic.a
      ${OGRE_ROOT}/lib/OGRE/libPlugin_OctreeSceneManagerStatic.a
      ${OGRE_ROOT}/lib/OGRE/libCodec_STBIStatic.a
      ${OGRE_ROOT}/lib/OGRE/libCodec_FreeImageStatic.a
      /opt/homebrew/lib/libfreeimage.dylib
      z)

     SET_TARGET_PROPERTIES(${YARS_TARGET} PROPERTIES LINK_FLAGS "-framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo")
      # "-framework OgreOverlay -framework RenderSystem_GL -framework Plugin_ParticleFX -framework OgreMain -framework Cocoa")
  endif(APPLE)

  if(UNIX AND NOT APPLE)
    target_link_libraries(${YARS_TARGET}
      X11 GL z Xt zzip freetype
  ${OGRE_LIBRARIES} ${OGRE_Overlay_LIBRARIES})

    endif(UNIX AND NOT APPLE)

  target_link_libraries(${YARS_TARGET} /opt/homebrew/lib/libSDL2.dylib)
  target_link_libraries(${YARS_TARGET} YarsView)
  target_link_libraries(${YARS_TARGET} YarsGUI)
endif(YARS_USE_VISUALISATION)

if(NOT APPLE AND YARS_USE_VISUALISATION)
  target_link_libraries(${YARS_TARGET}
    /usr/lib/x86_64-linux-gnu/libOgreMain.so
    /usr/lib/x86_64-linux-gnu/libOgreOverlay.so
    ${OGRE_RenderSystem_GL_LIBRARIES}
    ${OGRE_Plugin_ParticleFX_LIBRARIES}
    )
endif(NOT APPLE AND YARS_USE_VISUALISATION)

INSTALL(TARGETS ${YARS_TARGET} RUNTIME DESTINATION bin CONFIGURATIONS Release Debug)