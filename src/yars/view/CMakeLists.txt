add_library(YarsView ${LIBRARY_TYPE} YarsViewModel.cpp YarsViewControl.cpp)

add_subdirectory(gui)

include_directories(/usr/local/include/OGRE/)

if(LIBRARY_TYPE EQUAL STATIC)
  target_link_libraries(YarsView YarsGUI)
else(LIBRARY_TYPE EQUAL STATIC)
  target_link_libraries(YarsView YarsGUI quicktime)
endif(LIBRARY_TYPE EQUAL STATIC)


