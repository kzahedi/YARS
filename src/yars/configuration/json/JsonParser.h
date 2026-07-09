#ifndef YARS_CONFIGURATION_JSON_JSON_PARSER_H
#define YARS_CONFIGURATION_JSON_JSON_PARSER_H

#include <string>
#include <vector>

class DataRobotSimulationDescription;

namespace yars
{

/**
 * Replays a JSON config as the identical DataParseElement open/close
 * event stream that YarsXSDSaxHandler would emit for the equivalent XML
 * (see docs/planning/json-migration-notes.md for the full
 * characterization of that event contract). Feeds events into
 * `root->add(...)` exactly as the SAX handler does, so all Data* state
 * machines work unchanged.
 *
 * Accepted value shapes per key (legacy and concise forms mix freely):
 *  - scalar (string, number, bool): an attribute. Numbers and bools are
 *    stringified before reaching the atoi/atof/"true"-compare accessors,
 *    so native JSON types and the legacy all-strings form are equivalent.
 *  - scalar under an "elem_attr" key: shorthand for a child element with
 *    one attribute — "object_name": "main body" is
 *    <object name="main body"/>. Split at the first underscore; real
 *    element/attribute names never contain one.
 *  - object: a single child element
 *      "lookAt": {"x": 0.0, "y": 0.01, "z": 0.0}
 *  - array of objects: repeated child elements, one per entry
 *      "hinge": [{...}, {...}]
 *    (legacy always-arrays configs use this with one entry per singleton)
 *  - array of objects that carry "#tag": the key is a container element
 *    and the entries are its ordered, individually-tagged children —
 *      "sensors": [{"#tag": "ldr", ...}, {"#tag": "deflection", ...}]
 *    This is the flat form of the legacy {"#children": [...]} wrapper,
 *    which remains accepted (and is still required when the container
 *    element also carries attributes, e.g. <muscle name="...">).
 * Colour attribute values accept CSS-style "#RRGGBB[AA]" as well as bare
 * hex (stripped in DataColourFactory). scripts/json-canonicalize.py
 * rewrites any accepted config to the canonical concise shape.
 *
 * Further reader behaviour:
 *  - Line (//) and C-style block comments are permitted anywhere.
 *  - Duplicate object keys are a hard error (nlohmann would silently
 *    keep the last one, dropping an element without diagnostic).
 *  - `{"$include": "other.json", ...}` merges another JSON file (path
 *    relative to the including file): its object is taken as the base
 *    and sibling keys shallow-override it (an existing key is replaced
 *    at its original position, a new key is appended). Includes nest;
 *    cycles are a hard error. See xml/robots/ for shared robot
 *    fragments used this way.
 *
 * Root key is "yars" ("rosiml" accepted for legacy configs); either maps
 * to the internal "rosiml" element (YARS_STRING_ROSIML). The value may
 * be a plain object or a one-element array in the legacy shape. A
 * top-level "$schema" key is ignored by the reader; JSON Schemas for
 * editor validation live in schema/ (wired up for VS Code via
 * .vscode/settings.json, checkable headless via
 * scripts/json-schema-check.py).
 *
 * NOTE: unlike the XML path (YarsXSDSaxParser::read), this reader does not
 * support "-" (stdin) — stdin JSON configs are out of scope for Stage 1;
 * see YarsConfiguration::__readXmlFiles for where that is enforced.
 *
 * NOTE on validation: malformed/incomplete JSON structure (parse
 * errors, a missing "#tag" inside an ordered-children entry, a missing
 * root key) is caught here and reported cleanly via `errors`. A missing
 * REQUIRED attribute (e.g. a box face texture without `name`) is caught
 * by DataParseElement::requiredAttribute at the read site and reported
 * as "<file>: <element>: missing attribute '<name>'" through the same
 * `errors` path. Attributes read via the optional element->set()/
 * binding-table rows keep their legacy silently-default semantics; the
 * JSON Schema (schema/) additionally flags them while editing.
 */
bool parseJsonConfig(const std::string &jsonPath,
                      DataRobotSimulationDescription *root,
                      std::vector<std::string> &errors);

} // namespace yars

#endif // YARS_CONFIGURATION_JSON_JSON_PARSER_H
