set(SRC YarsMainControl.cpp RuntimeControl.cpp SignalHandler.cpp)

if(YARS_USE_VISUALISATION)
  set(SRC ${SRC} MainLoopThread.cpp)
endif(YARS_USE_VISUALISATION)

add_library(YarsMain ${LIBRARY_TYPE} ${SRC})

# Add Bullet Physics headers for YarsMainControl which includes physics headers
target_include_directories(YarsMain PRIVATE ${BULLET_INCLUDE_DIRS})

# Ensure proper C++ standard for this target
target_compile_features(YarsMain PRIVATE cxx_std_17)
