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
    if (it.value().is_array())
      continue; // handled as children below, not an attribute
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
      {
        if (!child.contains("#tag") || !child["#tag"].is_string())
          throw std::runtime_error(
              "JSON config: '#children' entry under <" + tag +
              "> is missing a string '#tag' field");
        emitElement(child["#tag"].get<std::string>(), child, root);
      }
    }
    else if (value.is_array())
    {
      for (const auto &child : value)
        emitElement(key, child, root);
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

    if (!document.contains("rosiml") || !document["rosiml"].is_array() ||
        document["rosiml"].empty())
    {
      errors.push_back(jsonPath +
                        ": missing top-level 'rosiml' element (root of a "
                        "JSON config must match XmlToJson's output shape)");
      return false;
    }

    emitElement("rosiml", document["rosiml"][0], root);
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
