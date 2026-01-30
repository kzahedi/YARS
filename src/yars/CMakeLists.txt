add_executable(${YARS_TARGET} yarsMain.cpp)

add_subdirectory(types)
add_subdirectory(configuration)
add_subdirectory(view/console)
add_subdirectory(physics)
add_subdirectory(util)
add_subdirectory(main)
add_subdirectory(logging)
add_subdirectory(rc)

target_link_libraries(${YARS_TARGET} ${XERCESC_LIBRARY_FILE})
target_link_libraries(${YARS_TARGET} ${Boost_LIBRARIES})
target_link_libraries(${YARS_TARGET} Boost::program_options Boost::thread)

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

  add_subdirectory(view)
  add_subdirectory(view/gui)

  # Add framework search path for OGRE
  set(OGRE_FRAMEWORK_DIR "${CMAKE_SOURCE_DIR}/ext/ogre/build/lib/macosx")
  target_link_options(${YARS_TARGET} PRIVATE "-F${OGRE_FRAMEWORK_DIR}")

  # Link OGRE libraries (set in cmake/IncludePackages.cmake)
  target_link_libraries(${YARS_TARGET} ${OGRE_LIBRARIES})

  # SDL2 (use variable from FindSDL2 module)
  target_link_libraries(${YARS_TARGET} ${SDL2_LIBRARY})

  # YARS view libraries
  target_link_libraries(${YARS_TARGET} YarsView)
  target_link_libraries(${YARS_TARGET} YarsGUI)

endif(YARS_USE_VISUALISATION)

INSTALL(TARGETS ${YARS_TARGET} RUNTIME DESTINATION bin CONFIGURATIONS Release Debug)
