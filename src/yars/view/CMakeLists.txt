add_subdirectory(gui)

set(GUI_HDRS YarsViewControl.h)
QT5_WRAP_CPP(qtview_MOC_SRCS ${GUI_HDRS})

add_library(YarsView ${LIBRARY_TYPE}
  YarsViewModel.cpp YarsViewControl.cpp ${qtview_MOC_SRCS})

qt5_use_modules(YarsView Core Widgets)

include_directories(/usr/local/include/OGRE/)

if(LIBRARY_TYPE EQUAL STATIC)
  target_link_libraries(YarsView YarsGUI)
else(LIBRARY_TYPE EQUAL STATIC)
  target_link_libraries(YarsView YarsGUI quicktime)
endif(LIBRARY_TYPE EQUAL STATIC)


