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

  // Walk children once, in document order, tracking contiguous runs of
  // same-tag siblings. Detect interleaving up front (rule 4) before
  // grouping anything into arrays (rule 2). Also reject non-whitespace
  // text content (rule 3).
  std::vector<std::string> childOrder;    // tag names, in first-seen order
  std::string lastTag;                    // tag of the previous element child

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

    if (tag != lastTag)
    {
      // Starting a new run. If this tag already appeared earlier under
      // this parent, the same-tag siblings are non-contiguous, i.e. some
      // other tag was interleaved between them. That reorders event
      // order once grouped into arrays, which is the exact hazard rule 4
      // protects against.
      for (const std::string &seenTag : childOrder)
      {
        if (seenTag == tag)
        {
          std::string parentName = transcode(element->getTagName());
          throw std::runtime_error(
              "xmlToJson: element <" + parentName + "> has non-contiguous "
              "(interleaved) <" + tag + "> siblings; grouping children by "
              "tag would silently reorder them. This configuration needs "
              "an explicit '#children' ordered representation (not yet "
              "implemented) — refusing to produce an order-lossy JSON "
              "document.");
        }
      }
      childOrder.push_back(tag);
      lastTag = tag;
    }

    nlohmann::ordered_json childJson = convertElement(childElement);
    object[tag].push_back(childJson);
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
