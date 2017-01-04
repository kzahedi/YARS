set(SRC YarsMainControl.cpp RuntimeControl.cpp SignalHandler.cpp)

if(YARS_USE_VISUALISATION)
  set(SRC ${SRC} MainLoopThread.cpp)
endif(YARS_USE_VISUALISATION)

add_library(YarsMain ${LIBRARY_TYPE} ${SRC})

