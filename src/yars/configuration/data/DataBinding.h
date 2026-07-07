#ifndef __DATA_BINDING_H__
#define __DATA_BINDING_H__

#include "DataNode.h"
#include "DataParseElement.h"

#include <functional>
#include <string>
#include <vector>

namespace yars
{

// One row per XML/JSON attribute of a Data* class. Replaces the
// hand-written attribute-parsing half of a Data*::add(DataParseElement*)
// implementation (child-element dispatch stays hand-written — it's a
// state machine that a flat table cannot model).
struct AttributeBinding
{
  const char *name;                                            // attribute name in config
  std::function<void(DataNode *, const std::string &)> apply;  // parse+assign
  bool required;
  const char *defaultValue;                                    // nullptr = no default
};

// Drives DataParseElement consumption from a table for the currently
// opening element:
//  - attribute present            -> binding.apply(self, attribute value)
//  - attribute absent, required   -> throws std::runtime_error with
//                                     "<element name>: missing attribute '<name>'"
//                                     (caller-side file-path prefixing, e.g.
//                                     JsonParser::parseJsonConfig's catch
//                                     block, turns this into the spec's
//                                     "file:node: missing attribute 'x'")
//  - attribute absent, defaulted  -> binding.apply(self, defaultValue)
//  - attribute absent, optional,
//    no default                  -> left untouched (matches legacy
//                                     DataParseElement::set() behaviour)
void applyAttributes(DataNode *self, DataParseElement *element,
                      const std::vector<AttributeBinding> &bindings);

} // namespace yars

#endif // __DATA_BINDING_H__
