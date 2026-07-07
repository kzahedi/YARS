#ifndef YARS_CONFIGURATION_JSON_XML_TO_JSON_H
#define YARS_CONFIGURATION_JSON_XML_TO_JSON_H

#include <nlohmann/json.hpp>
#include <string>

namespace yars
{

/**
 * Generic, mechanical XML → JSON conversion used by the JSON config-migration
 * Stage 0 converter.
 *
 * Conversion rules (see docs/planning for the full Stage 0 write-up):
 *   1. Every XML element becomes a JSON object. Attributes become
 *      string-valued fields verbatim — NO type coercion. The Data* classes
 *      do their own string parsing today; preserving strings preserves
 *      exact semantics, including locale/precision quirks.
 *   2. Child elements are ALWAYS represented as arrays keyed by tag name
 *      (e.g. "box": [ {...} ]), even when there is only a single child of
 *      that tag. This is uglier JSON but removes all ambiguity between
 *      "one child" and "many children", and makes the Stage 1 reader
 *      trivially symmetric with this converter.
 *   3. Non-whitespace element text content has no representation today —
 *      the legacy SAX handler has no characters() override, so such text
 *      never reaches the data layer even in the XML pipeline. Rather than
 *      invent a representation, the converter REJECTS (throws
 *      std::runtime_error) any element with non-whitespace text content.
 *   4. Sibling order across interleaved same-tag groups is load-bearing
 *      (e.g. DataRobot exposes sensors by arrival index, which is the
 *      controller I/O binding). Grouping children by tag name would
 *      silently reorder interleaved same-tag siblings
 *      (<proximity/><velocity/><proximity/> → both proximities adjacent).
 *      The converter therefore DETECTS non-contiguous same-tag siblings
 *      under any parent and fails loudly (throws std::runtime_error naming
 *      the parent element), rather than silently producing an
 *      order-lossy — and therefore semantically wrong — JSON document.
 *
 * Uses nlohmann::ordered_json (not nlohmann::json) throughout: insertion
 * order of object keys is load-bearing for rule 4's guarantee that
 * converter-produced files reproduce the original document's sibling order.
 */
nlohmann::ordered_json xmlToJson(const std::string &xmlPath);

} // namespace yars

#endif // YARS_CONFIGURATION_JSON_XML_TO_JSON_H
