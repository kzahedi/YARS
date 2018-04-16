set(SRC YarsMainControl.cpp RuntimeControl.cpp SignalHandler.cpp)

if(YARS_USE_VISUALISATION)
  set(SRC ${SRC} MainLoopThread.cpp)
endif(YARS_USE_VISUALISATION)

add_library(YarsMain ${LIBRARY_TYPE} ${SRC})


if(NOT APPLE)
  target_link_libraries(YarsMain boost_thread boost_program_options boost_filesystem boost_system)
endif(NOT APPLE)
