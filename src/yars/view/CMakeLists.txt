# Add codec framework headers for image loading support
if(APPLE)
  set(OGRE_ROOT ${CMAKE_SOURCE_DIR}/ext/ogre/install)
  include_directories(${OGRE_ROOT}/lib/Codec_STBIStatic.framework/Headers)
  include_directories(${OGRE_ROOT}/lib/Codec_FreeImageStatic.framework/Headers)
endif(APPLE)

add_library(YarsView ${LIBRARY_TYPE} YarsViewModel.cpp YarsViewControl.cpp)
