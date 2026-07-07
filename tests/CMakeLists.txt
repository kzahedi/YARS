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
  unit/test_circular_buffer.cpp
  unit/test_xml_to_json.cpp
  integration/test_simulation.cpp
)

# Link Google Test and minimal YARS libraries
target_link_libraries(
  yars_tests
  gtest
  gtest_main
  YarsDataTypes
  YarsUtil
  YarsConfiguration
)

# Include YARS headers
target_include_directories(yars_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/src/yars
)

# Bake source root into integration tests so they resolve project paths
# without walking up from cwd (build/xml is a symlink to xml/, which makes
# heuristic walks return the build dir as "project root").
target_compile_definitions(yars_tests PRIVATE
  YARS_PROJECT_ROOT="${CMAKE_SOURCE_DIR}"
)

# Add test discovery
include(GoogleTest)
gtest_discover_tests(yars_tests)

# Parse-all test driver — black-box: invokes the yars binary on every config
# enumerated in tests/xml_corpus.txt and asserts exit code 0 (parse + 1 iter).
# Built only if the yars binary is in the build.
add_executable(yars_xml_parse_all xml_parse_all.cpp)
target_link_libraries(yars_xml_parse_all gtest gtest_main)
add_dependencies(yars_xml_parse_all yars)
add_custom_target(test_xml_parse
  COMMAND ${CMAKE_COMMAND} -E env
    YARS_REPO_ROOT=${CMAKE_SOURCE_DIR}
    YARS_BIN=$<TARGET_FILE:yars>
    $<TARGET_FILE:yars_xml_parse_all>
  DEPENDS yars_xml_parse_all yars
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  USES_TERMINAL)

# XSD-validate driver — runs xmllint over the corpus against xml/yars.xsd.
add_custom_target(test_xsd_validate
  COMMAND ${CMAKE_SOURCE_DIR}/tests/xsd_validate.sh ${CMAKE_SOURCE_DIR}
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  USES_TERMINAL)

# Enable testing
enable_testing()