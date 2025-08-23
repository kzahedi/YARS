# Google Test Framework for YARS

# Download and configure Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add test executable
add_executable(
  yars_tests
  test_main.cpp
  types/test_p3d.cpp
  types/test_pose.cpp
  regression/test_braitenberg.cpp
)

# Link Google Test and minimal YARS libraries
target_link_libraries(
  yars_tests
  gtest
  gtest_main
  YarsDataTypes
  YarsUtil
)

# Include YARS headers
target_include_directories(yars_tests PRIVATE 
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/yars
)

# Add test discovery
include(GoogleTest)
gtest_discover_tests(yars_tests)

# Enable testing
enable_testing()