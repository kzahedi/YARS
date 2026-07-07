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

TEST(XmlToJson, InterleavedSameTagSiblingsBecomeOrderedChildren)
{
  const char *xml = "<root><a x=\"1\"/><b/><a x=\"2\"/></root>";
  std::string p = testing::TempDir() + "xmltojson_interleaved.xml";
  { std::ofstream f(p); f << xml; }
  auto j = yars::xmlToJson(p);
  auto &r = j["root"][0];

  ASSERT_TRUE(r.contains("#children"));
  ASSERT_FALSE(r.contains("a"));   // grouped-by-tag form must not also appear
  ASSERT_FALSE(r.contains("b"));

  auto &children = r["#children"];
  ASSERT_EQ(children.size(), 3u);
  EXPECT_EQ(children[0]["#tag"], "a");
  EXPECT_EQ(children[0]["x"], "1");
  EXPECT_EQ(children[1]["#tag"], "b");
  EXPECT_EQ(children[2]["#tag"], "a");
  EXPECT_EQ(children[2]["x"], "2");   // document order preserved, not grouped
}

TEST(XmlToJson, MixedContiguousParentsKeepArrayFormInterleavedGetChildren)
{
  // <outer> has contiguous <same> runs (array form preserved) but its
  // <mid> child has interleaved <a>/<b> siblings (#children form).
  const char *xml =
      "<outer>"
      "<same n=\"1\"/><same n=\"2\"/>"
      "<mid><a x=\"1\"/><b/><a x=\"2\"/></mid>"
      "</outer>";
  std::string p = testing::TempDir() + "xmltojson_mixed.xml";
  { std::ofstream f(p); f << xml; }
  auto j = yars::xmlToJson(p);
  auto &outer = j["outer"][0];

  ASSERT_FALSE(outer.contains("#children"));
  ASSERT_TRUE(outer.contains("same"));
  EXPECT_EQ(outer["same"].size(), 2u);
  ASSERT_TRUE(outer.contains("mid"));

  auto &mid = outer["mid"][0];
  ASSERT_TRUE(mid.contains("#children"));
  EXPECT_EQ(mid["#children"].size(), 3u);
  EXPECT_EQ(mid["#children"][0]["#tag"], "a");
  EXPECT_EQ(mid["#children"][1]["#tag"], "b");
  EXPECT_EQ(mid["#children"][2]["#tag"], "a");
}
