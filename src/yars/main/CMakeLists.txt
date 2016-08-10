if(YARS_USE_VISUALISATION)
  set(GUI_HDRS YarsApplication.h MainLoopThread.h)
  QT5_WRAP_CPP(qtview_MOC_SRCS ${GUI_HDRS})
  add_library(YarsMain         ${LIBRARY_TYPE} YarsMainControl.cpp
                                               MainLoopThread.cpp
                                               YarsApplication.cpp
                                               RuntimeControl.cpp
                                               SignalHandler.cpp
                                               ${qtview_MOC_SRCS})

  qt5_use_modules(YarsMain Core Widgets)
else(YARS_USE_VISUALISATION)
  add_library(YarsMain         ${LIBRARY_TYPE} YarsMainControl.cpp
                                               RuntimeControl.cpp
                                               SignalHandler.cpp)
endif(YARS_USE_VISUALISATION)

