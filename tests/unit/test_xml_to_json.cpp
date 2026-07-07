#include <gtest/gtest.h>
#include <yars/configuration/json/XmlToJson.h>
#include <fstream>

TEST(XmlToJson, AttributesBecomeStringFieldsChildrenBecomeArrays)
{
  const char *xml = "<root a=\"1.5\"><child b=\"x\"/><child b=\"y\"/></root>";
  std::string p = testing::TempDir() + "xmltojson_test.xml";
  { std::ofstream f(p); f << xml; }
  auto j = yars::xmlToJson(p);
  EXPECT_EQ(j["root"][0]["a"], "1.5");
  EXPECT_EQ(j["root"][0]["child"].size(), 2u);
  EXPECT_EQ(j["root"][0]["child"][1]["b"], "y");
}

TEST(XmlToJson, InterleavedSameTagSiblingsFailLoudly)
{
  const char *xml = "<root><a x=\"1\"/><b/><a x=\"2\"/></root>";
  std::string p = testing::TempDir() + "xmltojson_interleaved.xml";
  { std::ofstream f(p); f << xml; }
  EXPECT_THROW(yars::xmlToJson(p), std::runtime_error);   // order-lossy input must be rejected
}
