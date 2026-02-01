#include <catch2/catch_test_macros.hpp>

#include <yars/configuration/data/Data.h>
#include <yars/configuration/YarsConfiguration.h>

TEST_CASE("Data singleton exists", "[Configuration]") {
  Data* data = Data::instance();
  REQUIRE(data != nullptr);
}

TEST_CASE("YarsConfiguration singleton exists", "[Configuration]") {
  YarsConfiguration* config = YarsConfiguration::instance();
  REQUIRE(config != nullptr);
}

// Note: More comprehensive configuration tests would require
// actual XML files and full initialization which is complex.
// These basic tests verify the infrastructure works.
