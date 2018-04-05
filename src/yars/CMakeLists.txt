add_executable(${YARS_TARGET} yarsMain.cpp)

add_subdirectory(types)
add_subdirectory(configuration)
add_subdirectory(view/console)
add_subdirectory(physics)
add_subdirectory(util)
add_subdirectory(main)
add_subdirectory(logging)
add_subdirectory(rc)

target_link_libraries(${YARS_TARGET} ${XERCESC_LIBRARY_FILE}) # ${Boost_LIBRARIES}) # ${BULLET_LIBRARIES})
target_link_libraries(${YARS_TARGET} m stdc++ pthread dl)
target_link_libraries(${YARS_TARGET} YarsMain)
target_link_libraries(${YARS_TARGET} YarsConfiguration)
target_link_libraries(${YARS_TARGET} YarsPhysics)
target_link_libraries(${YARS_TARGET} YarsUtil)
target_link_libraries(${YARS_TARGET} YarsLogging)
target_link_libraries(${YARS_TARGET} YarsViewConsole)
target_link_libraries(${YARS_TARGET} YarsDataTypes)
target_link_libraries(${YARS_TARGET} boost_program_options)

if(YARS_USE_CAPTURE_VIDEO)
  target_link_libraries(${YARS_TARGET} avutil avcodec avfilter avresample postproc
    swscale avdevice avformat swresample)
endif(YARS_USE_CAPTURE_VIDEO)

if(YARS_USE_VISUALISATION)

  add_subdirectory(view)
  add_subdirectory(view/gui)

  if(APPLE)
    SET_TARGET_PROPERTIES(${YARS_TARGET} PROPERTIES LINK_FLAGS
      "-framework OgreOverlay -framework RenderSystem_GL -framework Plugin_ParticleFX -framework Ogre -framework Cocoa")
  endif(APPLE)
  if(UNIX AND NOT APPLE)
    # message("*************")
    # message(${OGRE_LIBRARIES})
    # message(${OGRE_Overlay_LIBRARIES})
    # message(${OGRE_RenderSystem_GL_LIBRARIES})
    # message(${OGRE_Plugin_ParticleFX_LIBRARIES})
    # message("*************")
    message("/usr/lib/x86_64-linux-gnu/libOgreMain.so /usr/lib/x86_64-linux-gnu/libOgreOverlay.so")

    target_link_libraries(${YARS_TARGET}
      X11 GL z Xt zzip freeimage freetype
      # ${OGRE_RenderSystem_GL_LIBRARIES}
      # ${OGRE_Plugin_ParticleFX_LIBRARIES}
      )

    target_link_libraries(${YARS_TARGET}
      /usr/lib/x86_64-linux-gnu/libOgreMain.so
      /usr/lib/x86_64-linux-gnu/libOgreOverlay.so
      )
  endif(UNIX AND NOT APPLE)

  target_link_libraries(${YARS_TARGET} SDL2)
  target_link_libraries(${YARS_TARGET} YarsView)
  target_link_libraries(${YARS_TARGET} YarsGUI)
endif(YARS_USE_VISUALISATION)

if(NOT APPLE)
  target_link_libraries(${YARS_TARGET}
    /usr/lib/x86_64-linux-gnu/libOgreMain.so
    /usr/lib/x86_64-linux-gnu/libOgreOverlay.so
    ${OGRE_RenderSystem_GL_LIBRARIES}
    ${OGRE_Plugin_ParticleFX_LIBRARIES}
    )
endif(NOT APPLE)

INSTALL(TARGETS ${YARS_TARGET} RUNTIME DESTINATION bin CONFIGURATIONS Release Debug)
