#include <yars/configuration/json/JsonParser.h>

#include <yars/configuration/data/DataParseAttribute.h>
#include <yars/configuration/data/DataParseElement.h>
#include <yars/configuration/data/DataRobotSimulationDescription.h>

#include <nlohmann/json.hpp>

#include <fstream>
#include <sstream>

using nlohmann::ordered_json;

namespace yars
{

namespace
{

// All attribute values must be stringified before being handed to
// DataParseAttribute::setValue — the Data* classes' typed accessors
// (intValue/realValue/boolValue) do atoi/atof/"true"-compare on the stored
// string (see docs/planning/json-migration-notes.md §1). XmlToJson always
// emits strings for attribute fields, but be robust to a hand-written or
// hand-edited JSON config that used a native JSON number/bool.
std::string toAttributeString(const ordered_json &value)
{
  if (value.is_string())
    return value.get<std::string>();
  if (value.is_boolean())
    return value.get<bool>() ? "true" : "false";
  // Numbers (and anything else scalar): dump() renders a plain textual
  // form ("3.5", "12", ...) that atof/atoi parse identically to what an
  // XML attribute string would have looked like.
  return value.dump();
}

// A key like "object_name" is shorthand for a child element with a
// single attribute: <object name="..."/>. Split at the first underscore;
// element and attribute names themselves never contain one.
bool splitElementAttributeKey(const std::string &key, std::string &element,
                               std::string &attribute)
{
  const std::size_t underscore = key.find('_');
  if (underscore == std::string::npos || underscore == 0 ||
      underscore + 1 == key.size() || key[0] == '#')
    return false;
  element = key.substr(0, underscore);
  attribute = key.substr(underscore + 1);
  return true;
}

// True when an array value expresses an ordered container of
// heterogeneous children (each entry carries its own "#tag"), i.e. the
// flattened form of the legacy {"#children": [...]} wrapper.
bool isTaggedChildArray(const ordered_json &value)
{
  return value.is_array() && !value.empty() && value.front().is_object() &&
         value.front().contains("#tag");
}

void emitElement(const std::string &tag, const ordered_json &object,
                  DataRobotSimulationDescription *root);

// Emits one "#children"-style entry: an object whose "#tag" field names
// the element.
void emitTaggedChild(const std::string &parentTag, const ordered_json &child,
                      DataRobotSimulationDescription *root)
{
  if (!child.is_object() || !child.contains("#tag") ||
      !child["#tag"].is_string())
    throw std::runtime_error(
        "JSON config: ordered-children entry under <" + parentTag +
        "> is missing a string '#tag' field");
  emitElement(child["#tag"].get<std::string>(), child, root);
}

// Emits the opening/closing DataParseElement event pair for one JSON
// object representing an element (either a plain child-array entry or a
// "#children" entry), then recurses into its own children, mirroring the
// exact sequence YarsXSDSaxHandler::startElement/endElement would produce
// for the equivalent XML (docs/planning/json-migration-notes.md §1-3).
//
// Lifetime rules replayed exactly per the notes: the opening
// DataParseElement is leaked (never deleted) to match the SAX handler's
// (confirmed) leak — downstream Data*::add() implementations were audited
// against that assumption, not against prompt cleanup. The closing
// DataParseElement is deleted immediately after root->add() returns, also
// matching the SAX handler.
void emitElement(const std::string &tag, const ordered_json &object,
                  DataRobotSimulationDescription *root)
{
  DataParseElement *opening =
      new DataParseElement(YARS_DATA_PARSE_ELEMENT_TYPE_OPENING);
  opening->setName(tag);

  for (auto it = object.begin(); it != object.end(); ++it)
  {
    const std::string &key = it.key();
    if (key == "#tag" || key == "#children")
      continue;
    if (it.value().is_array() || it.value().is_object())
      continue; // handled as children below, not an attribute
    std::string childTag, childAttribute;
    if (splitElementAttributeKey(key, childTag, childAttribute))
      continue; // "elem_attr" shorthand: a child element, handled below
    DataParseAttribute *attribute = new DataParseAttribute();
    attribute->setName(key);
    attribute->setValue(toAttributeString(it.value()));
    opening->add(attribute);
  }

  root->add(opening);

  for (auto it = object.begin(); it != object.end(); ++it)
  {
    const std::string &key = it.key();
    const ordered_json &value = it.value();

    if (key == "#children")
    {
      for (const auto &child : value)
        emitTaggedChild(tag, child, root);
    }
    else if (isTaggedChildArray(value))
    {
      // Flattened ordered container: `"sensors": [{"#tag": "ldr", ...},
      // ...]` — the key is an element whose ordered, per-entry-tagged
      // children are the array entries (concise form of
      // `"sensors": {"#children": [...]}`).
      DataParseElement *containerOpening =
          new DataParseElement(YARS_DATA_PARSE_ELEMENT_TYPE_OPENING);
      containerOpening->setName(key);
      root->add(containerOpening);
      for (const auto &child : value)
        emitTaggedChild(key, child, root);
      DataParseElement *containerClosing =
          new DataParseElement(YARS_DATA_PARSE_ELEMENT_TYPE_CLOSING);
      containerClosing->setName(key);
      root->add(containerClosing);
      delete containerClosing;
    }
    else if (value.is_array())
    {
      for (const auto &child : value)
        emitElement(key, child, root);
    }
    else if (value.is_object())
    {
      // Concise singleton form: `"lookAt": {"x": ...}` is equivalent to
      // the always-arrays form `"lookAt": [{"x": ...}]`.
      emitElement(key, value, root);
    }
    else
    {
      // Scalar with an "elem_attr" key: shorthand for a child element
      // with one attribute, e.g. `"object_name": "main body"` for
      // `"object": {"name": "main body"}`.
      std::string childTag, childAttribute;
      if (splitElementAttributeKey(key, childTag, childAttribute))
      {
        DataParseElement *childOpening =
            new DataParseElement(YARS_DATA_PARSE_ELEMENT_TYPE_OPENING);
        childOpening->setName(childTag);
        DataParseAttribute *attribute = new DataParseAttribute();
        attribute->setName(childAttribute);
        attribute->setValue(toAttributeString(value));
        childOpening->add(attribute);
        root->add(childOpening);
        DataParseElement *childClosing =
            new DataParseElement(YARS_DATA_PARSE_ELEMENT_TYPE_CLOSING);
        childClosing->setName(childTag);
        root->add(childClosing);
        delete childClosing;
      }
    }
  }

  DataParseElement *closing =
      new DataParseElement(YARS_DATA_PARSE_ELEMENT_TYPE_CLOSING);
  closing->setName(tag);
  root->add(closing);
  delete closing;
}

std::string readWholeFile(const std::string &path)
{
  std::ifstream file(path, std::ios::in | std::ios::binary);
  if (!file)
    throw std::runtime_error("JSON config: could not open '" + path + "'");
  std::ostringstream contents;
  contents << file.rdbuf();
  return contents.str();
}

} // namespace

bool parseJsonConfig(const std::string &jsonPath,
                      DataRobotSimulationDescription *root,
                      std::vector<std::string> &errors)
{
  try
  {
    // Stdin ("-") is intentionally unsupported here — see JsonParser.h and
    // YarsConfiguration::__readXmlFiles, where ".json" format detection
    // never routes "-" to this function.
    std::string text = readWholeFile(jsonPath);
    ordered_json document = ordered_json::parse(text);

    // Canonical root key is "yars"; "rosiml" is the legacy name from the
    // XML era. Either maps to the internal "rosiml" element the Data*
    // dispatch expects (YARS_STRING_ROSIML).
    const char *rootKey = document.contains("yars") ? "yars" : "rosiml";
    const bool rootIsObject =
        document.contains(rootKey) && document[rootKey].is_object();
    const bool rootIsArray = document.contains(rootKey) &&
                             document[rootKey].is_array() &&
                             !document[rootKey].empty();
    if (!rootIsObject && !rootIsArray)
    {
      errors.push_back(jsonPath +
                        ": missing top-level 'yars' element (or legacy "
                        "'rosiml'); must be an object, or an array in the "
                        "legacy always-arrays shape");
      return false;
    }

    emitElement("rosiml",
                rootIsObject ? document[rootKey] : document[rootKey][0],
                root);
    return true;
  }
  catch (const nlohmann::json::exception &e)
  {
    errors.push_back(jsonPath + ": JSON error: " + e.what());
    return false;
  }
  catch (const std::exception &e)
  {
    errors.push_back(jsonPath + ": " + e.what());
    return false;
  }
}

} // namespace yars
