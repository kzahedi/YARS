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
 * Two element shapes are accepted per key, and may be mixed freely:
 *  - concise: an object value is a single child element
 *      "lookAt": {"x": 0.0, "y": 0.01, "z": 0.0}
 *  - legacy always-arrays (XmlToJson's Stage-0 output): an array value
 *    holds one child element per entry
 *      "lookAt": [{"x": "0.0", "y": "0.01", "z": "0.0"}]
 *    Arrays remain the only way to express repeated children.
 * Scalar values (string, number, bool) are attributes; numbers and bools
 * are stringified before reaching the atoi/atof/"true"-compare accessors,
 * so native JSON types and the all-strings legacy form are equivalent.
 * scripts/json-canonicalize.py rewrites a legacy config to the concise
 * typed shape.
 *
 * Root element must be "rosiml" (matching YARS_STRING_ROSIML) — either a
 * plain object, or a one-element array in the legacy shape.
 *
 * NOTE: unlike the XML path (YarsXSDSaxParser::read), this reader does not
 * support "-" (stdin) — stdin JSON configs are out of scope for Stage 1;
 * see YarsConfiguration::__readXmlFiles for where that is enforced.
 *
 * NOTE on validation (Stage 1 known gap, see docs/planning/json-migration-notes.md
 * §"JSON validation gap"): malformed/incomplete JSON structure (parse
 * errors, a missing "#tag" inside a "#children" entry, a missing root
 * "rosiml" key) is caught here and reported cleanly via `errors`. A
 * missing REQUIRED XML attribute (e.g. <mass kg="..."/> without `kg`) is
 * NOT guarded here — same as the XML path, which relies on upstream XSD
 * validation to prevent it from ever reaching the Data* classes. The JSON
 * path has no equivalent schema validation yet (planned for Stage 4's
 * binding tables), so a malformed-but-JSON-well-formed config can still
 * crash inside a Data*::add() unchecked `element->attribute(x)->value()`
 * dereference. This is a documented, user-visible gap for Stage 1.
 */
bool parseJsonConfig(const std::string &jsonPath,
                      DataRobotSimulationDescription *root,
                      std::vector<std::string> &errors);

} // namespace yars

#endif // YARS_CONFIGURATION_JSON_JSON_PARSER_H
