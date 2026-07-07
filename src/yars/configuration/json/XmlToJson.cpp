#include <yars/configuration/json/XmlToJson.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace xercesc;

namespace yars
{

namespace
{

// RAII helper for XMLPlatformUtils::Initialize()/Terminate(). Xerces
// tolerates nested Initialize()/Terminate() pairs (it reference-counts
// internally), so this is safe to use even though other call sites
// (YarsXSDSaxParser, YarsXSDGenerator) also Initialize/Terminate around
// their own parses.
class XercesGuard
{
public:
  XercesGuard() { XMLPlatformUtils::Initialize(); }
  ~XercesGuard() { XMLPlatformUtils::Terminate(); }
};

std::string transcode(const XMLCh *s)
{
  if (s == nullptr)
    return std::string();
  char *narrow = XMLString::transcode(s);
  std::string result(narrow);
  XMLString::release(&narrow);
  return result;
}

bool isAllWhitespace(const std::string &s)
{
  for (char c : s)
  {
    if (!std::isspace(static_cast<unsigned char>(c)))
      return false;
  }
  return true;
}

// Recursively converts a DOMElement into a JSON object per the rules
// documented in XmlToJson.h. Throws std::runtime_error on:
//   - non-whitespace text content on any element
//   - non-contiguous (interleaved) same-tag child siblings under any parent
nlohmann::ordered_json convertElement(DOMElement *element)
{
  nlohmann::ordered_json object = nlohmann::ordered_json::object();

  // Attributes -> string fields, verbatim, no type coercion.
  DOMNamedNodeMap *attributes = element->getAttributes();
  if (attributes != nullptr)
  {
    XMLSize_t count = attributes->getLength();
    for (XMLSize_t i = 0; i < count; ++i)
    {
      DOMNode *attributeNode = attributes->item(i);
      std::string name = transcode(attributeNode->getNodeName());
      std::string value = transcode(attributeNode->getNodeValue());
      object[name] = value;
    }
  }

  // Walk children once, in document order, collecting (tag, converted-json)
  // pairs. Also reject non-whitespace text content (rule 3). Grouping into
  // either the array-by-tag representation (rule 2) or the ordered
  // '#children' representation (rule 4 amendment) happens in a second pass
  // below, once we know whether any same-tag siblings are non-contiguous.
  struct ChildEntry
  {
    std::string tag;
    nlohmann::ordered_json json;
  };
  std::vector<ChildEntry> children;

  for (DOMNode *child = element->getFirstChild(); child != nullptr;
       child = child->getNextSibling())
  {
    short type = child->getNodeType();

    if (type == DOMNode::TEXT_NODE || type == DOMNode::CDATA_SECTION_NODE)
    {
      std::string text = transcode(child->getNodeValue());
      if (!isAllWhitespace(text))
      {
        std::string elementName = transcode(element->getTagName());
        throw std::runtime_error(
            "xmlToJson: element <" + elementName + "> has non-whitespace "
            "text content, which has no JSON representation: \"" + text +
            "\"");
      }
      continue;
    }

    if (type != DOMNode::ELEMENT_NODE)
      continue;

    DOMElement *childElement = static_cast<DOMElement *>(child);
    std::string tag = transcode(childElement->getTagName());
    children.push_back({tag, convertElement(childElement)});
  }

  // Detect whether any same-tag siblings are non-contiguous (interleaved
  // with a different tag in between). If so, this parent's children are
  // represented as an ordered '#children' array instead of grouped-by-tag
  // arrays, preserving document order exactly (rule 4 amendment).
  bool interleaved = false;
  {
    std::vector<std::string> seenTags;
    std::string lastTag;
    for (const ChildEntry &entry : children)
    {
      if (entry.tag == lastTag)
        continue;
      for (const std::string &seenTag : seenTags)
      {
        if (seenTag == entry.tag)
        {
          interleaved = true;
          break;
        }
      }
      if (interleaved)
        break;
      seenTags.push_back(entry.tag);
      lastTag = entry.tag;
    }
  }

  if (interleaved)
  {
    nlohmann::ordered_json orderedChildren = nlohmann::ordered_json::array();
    for (const ChildEntry &entry : children)
    {
      // "#tag" is emitted first so a reader can recover the element name
      // before seeing any of the element's own attributes/children.
      nlohmann::ordered_json wrapped = nlohmann::ordered_json::object();
      wrapped["#tag"] = entry.tag;
      for (auto it = entry.json.begin(); it != entry.json.end(); ++it)
        wrapped[it.key()] = it.value();
      orderedChildren.push_back(wrapped);
    }
    object["#children"] = orderedChildren;
  }
  else
  {
    for (const ChildEntry &entry : children)
      object[entry.tag].push_back(entry.json);
  }

  return object;
}

} // namespace

nlohmann::ordered_json xmlToJson(const std::string &xmlPath)
{
  XercesGuard xercesGuard;

  std::unique_ptr<XercesDOMParser> parser(new XercesDOMParser());
  parser->setValidationScheme(XercesDOMParser::Val_Never);
  parser->setDoNamespaces(false);
  parser->setDoSchema(false);
  parser->setLoadExternalDTD(false);

  try
  {
    parser->parse(xmlPath.c_str());
  }
  catch (const XMLException &toCatch)
  {
    throw std::runtime_error("xmlToJson: XML error parsing '" + xmlPath +
                              "': " + transcode(toCatch.getMessage()));
  }
  catch (const DOMException &toCatch)
  {
    throw std::runtime_error("xmlToJson: DOM error parsing '" + xmlPath +
                              "': " + transcode(toCatch.getMessage()));
  }
  catch (const SAXParseException &toCatch)
  {
    throw std::runtime_error("xmlToJson: SAX parse error in '" + xmlPath +
                              "': " + transcode(toCatch.getMessage()));
  }

  DOMDocument *document = parser->getDocument();
  if (document == nullptr)
    throw std::runtime_error("xmlToJson: failed to parse '" + xmlPath + "'");

  DOMElement *root = document->getDocumentElement();
  if (root == nullptr)
    throw std::runtime_error("xmlToJson: '" + xmlPath + "' has no root element");

  nlohmann::ordered_json result = nlohmann::ordered_json::object();
  std::string rootTag = transcode(root->getTagName());
  result[rootTag].push_back(convertElement(root));

  return result;
}

} // namespace yars
