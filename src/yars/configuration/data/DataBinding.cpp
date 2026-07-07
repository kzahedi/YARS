#include "DataBinding.h"

#include <stdexcept>

namespace yars
{

void applyAttributes(DataNode *self, DataParseElement *element,
                      const std::vector<AttributeBinding> &bindings)
{
  for (const AttributeBinding &binding : bindings)
  {
    DataParseAttribute *attribute = element->attribute(binding.name);
    if (attribute != nullptr)
    {
      binding.apply(self, attribute->value());
    }
    else if (binding.required)
    {
      throw std::runtime_error(element->name() + ": missing attribute '" +
                                binding.name + "'");
    }
    else if (binding.defaultValue != nullptr)
    {
      binding.apply(self, std::string(binding.defaultValue));
    }
    // else: optional, no default — leave the field at its current value,
    // matching legacy DataParseElement::set() no-op-on-absent semantics.
  }
}

} // namespace yars
