# YARS Test Suite
# Uses Catch2 for unit and integration testing

find_package(Catch2 3 REQUIRED)

include(CTest)
include(Catch)

# Unit tests
add_executable(yars_unit_tests
  unit/test_p3d.cpp
  unit/test_quaternion.cpp
  unit/test_matrix.cpp
  unit/test_circular_buffer.cpp
  unit/test_string_tokeniser.cpp
  unit/test_colour.cpp
)

target_include_directories(yars_unit_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(yars_unit_tests PRIVATE
  Catch2::Catch2WithMain
  YarsDataTypes
  YarsUtil
)

# Register tests with CTest
catch_discover_tests(yars_unit_tests)

# Integration tests (require more dependencies)
add_executable(yars_integration_tests
  integration/test_configuration.cpp
  integration/test_simulation.cpp
)

target_include_directories(yars_integration_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(yars_integration_tests PRIVATE
  Catch2::Catch2WithMain
  YarsConfiguration
  YarsDataTypes
  YarsUtil
  YarsViewConsole
  YarsGUI
)

catch_discover_tests(yars_integration_tests)
