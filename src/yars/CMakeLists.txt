#if(APPLE)
#add_executable(${YARS_TARGET} MACOSX_BUNDLE yarsMain.cpp)
#else(APPLE)
  add_executable(${YARS_TARGET} yarsMain.cpp)
#endif(APPLE)

add_subdirectory(types)
add_subdirectory(configuration)
add_subdirectory(view/console)
add_subdirectory(physics)
add_subdirectory(util)
add_subdirectory(main)
add_subdirectory(logging)
add_subdirectory(rc)


include_directories(.)

INSTALL(TARGETS ${YARS_TARGET}
  RUNTIME DESTINATION bin
  CONFIGURATIONS Release Debug)


target_link_libraries(${YARS_TARGET} ${XERCESC_LIBRARY_FILE}) # ${Boost_LIBRARIES}) # ${BULLET_LIBRARIES})

target_link_libraries(${YARS_TARGET} m stdc++ pthread dl)

IF(WINDOWS OR MINGW)
  target_link_libraries(${YARS_TARGET} ${ODE_LIBRARY_FILE} Ws2_32.lib)
ENDIF(WINDOWS OR MINGW)

if(YARS_USE_CAPTURE_VIDEO)
  target_link_libraries(${YARS_TARGET} quicktime)
endif(YARS_USE_CAPTURE_VIDEO)

if(YARS_USE_VISUALISATION)
  add_subdirectory(view)
  target_link_libraries(${YARS_TARGET} YarsView)

  link_directories("/usr/local/lib/")
  if(UNIX AND NOT APPLE)
    link_directories(/usr/local/lib /usr/local/lib/OGRE)
    target_link_libraries(${YARS_TARGET}
      X11 GL z Xt pthread ${OGRE_LIBRARIES}
      zzip freeimage freetype quicktime)
      # zzip freeimage freetype GLEW quicktime)

  else(UNIX AND NOT APPLE)
    target_link_libraries(${YARS_TARGET} zzip freeimage freetype)
      # zzip freeimage freetype GLEW)
    # Plugin_CgProgramManagerStatic
  endif(UNIX AND NOT APPLE)
  target_link_libraries(${YARS_TARGET} SDL2)

  # message("Ogre: ${OGRE_LIBRARIES}")
  # target_link_libraries(${YARS_TARGET} ${OGRE_LIBRARIES} zzip freeimage freetype GLEW)

  if(APPLE)
    SET_TARGET_PROPERTIES(${YARS_TARGET} PROPERTIES LINK_FLAGS
      "-framework OgreOverlay -framework RenderSystem_GL -framework Plugin_ParticleFX -framework Ogre -framework Cocoa")
  else(APPLE)
    message("Ogre: ${OGRE_LIBRARIES}")
    #target_link_libraries(${YARS_TARGET} ${OGRE_LIBRARIES} zzip freeimage freetype GLEW)
    target_link_libraries(${YARS_TARGET} ${OGRE_LIBRARIES} zzip freeimage freetype GLEW)
  endif(APPLE)

endif(YARS_USE_VISUALISATION)

target_link_libraries(${YARS_TARGET} YarsMain)
target_link_libraries(${YARS_TARGET} YarsConfiguration)
target_link_libraries(${YARS_TARGET} YarsPhysics)
target_link_libraries(${YARS_TARGET} YarsUtil)
target_link_libraries(${YARS_TARGET} YarsLogging)
target_link_libraries(${YARS_TARGET} YarsViewConsole)
target_link_libraries(${YARS_TARGET} YarsDataTypes)
target_link_libraries(${YARS_TARGET} boost_program_options)
