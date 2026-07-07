#include <gtest/gtest.h>

#include <yars/configuration/data/DataBinding.h>
#include <yars/configuration/data/DataNode.h>
#include <yars/configuration/data/DataParseAttribute.h>
#include <yars/configuration/data/DataParseElement.h>

namespace
{

// Minimal DataNode subclass exercising two bindings: one required
// attribute with no default, one optional attribute with a default.
class FakeNode : public DataNode
{
  public:
    FakeNode() : DataNode(nullptr) {}

    void add(DataParseElement *) override {}

    std::string requiredValue;
    std::string optionalValue;
};

DataParseElement *makeElement(const std::string &tagName)
{
  DataParseElement *element =
      new DataParseElement(YARS_DATA_PARSE_ELEMENT_TYPE_OPENING);
  element->setName(tagName);
  return element;
}

void addAttribute(DataParseElement *element, const std::string &name,
                   const std::string &value)
{
  DataParseAttribute *attribute = new DataParseAttribute();
  attribute->setName(name);
  attribute->setValue(value);
  element->add(attribute);
}

std::vector<yars::AttributeBinding> bindings()
{
  return {
      {"required_attr",
       [](DataNode *self, const std::string &value) {
         static_cast<FakeNode *>(self)->requiredValue = value;
       },
       /*required=*/true, /*defaultValue=*/nullptr},
      {"optional_attr",
       [](DataNode *self, const std::string &value) {
         static_cast<FakeNode *>(self)->optionalValue = value;
       },
       /*required=*/false, /*defaultValue=*/"fallback"},
  };
}

} // namespace

TEST(ApplyAttributes, PresentAttributesLandOnTheNode)
{
  FakeNode node;
  DataParseElement *element = makeElement("fake");
  addAttribute(element, "required_attr", "hello");
  addAttribute(element, "optional_attr", "world");

  yars::applyAttributes(&node, element, bindings());

  EXPECT_EQ(node.requiredValue, "hello");
  EXPECT_EQ(node.optionalValue, "world");

  delete element;
}

TEST(ApplyAttributes, MissingOptionalAttributeFallsBackToDefault)
{
  FakeNode node;
  DataParseElement *element = makeElement("fake");
  addAttribute(element, "required_attr", "hello");

  yars::applyAttributes(&node, element, bindings());

  EXPECT_EQ(node.requiredValue, "hello");
  EXPECT_EQ(node.optionalValue, "fallback");

  delete element;
}

TEST(ApplyAttributes, MissingRequiredAttributeThrowsWithFileNodeMessageFormat)
{
  FakeNode node;
  DataParseElement *element = makeElement("fake");
  addAttribute(element, "optional_attr", "world");

  bool threw = false;
  try
  {
    yars::applyAttributes(&node, element, bindings());
  }
  catch (const std::runtime_error &e)
  {
    threw = true;
    // "file:node: message" format: JsonParser's catch block prefixes this
    // with "<path>: ", so the string thrown here must itself already read
    // "<node>: missing attribute '<name>'".
    EXPECT_STREQ(e.what(), "fake: missing attribute 'required_attr'");
  }
  EXPECT_TRUE(threw);

  delete element;
}
