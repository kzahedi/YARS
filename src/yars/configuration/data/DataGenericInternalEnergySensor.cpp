#include "DataGenericInternalEnergySensor.h"
#include "DataBinding.h"
#include "DataDomainFactory.h"

# define YARS_STRING_MAPPING            (char*)"mapping"
# define YARS_STRING_POSE               (char*)"pose"
# define YARS_STRING_NAME               (char*)"name"
# define YARS_STRING_MIN_MAX_DEFINITION (char*)"min_max_definition"


namespace
{
// Attribute binding table for the internal energy sensor's own opening tag.
// Child-element dispatch stays hand-written below.
const std::vector<yars::AttributeBinding> &genericInternalEnergySensorAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_NAME,
       [](DataNode *self, const std::string &value)
       { static_cast<DataGenericInternalEnergySensor *>(self)->setName(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

DataGenericInternalEnergySensor::DataGenericInternalEnergySensor(DataNode* parent)
  : DataSensor(parent, DATA_GENERIC_INTERNAL_ENERGY_SENSOR)
{ }

DataGenericInternalEnergySensor::~DataGenericInternalEnergySensor()
{
}

void DataGenericInternalEnergySensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GENERIC_INTERNAL_ENERGY_SENSOR))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_GENERIC_INTERNAL_ENERGY_SENSOR))
  {
    yars::applyAttributes(this, element, genericInternalEnergySensorAttributeBindings());
  }
  if(element->opening(YARS_STRING_NOISE))
  {
    _noise  = std::make_unique<DataNoise>(this);
    current = _noise.get();
    _noise->add(element);
  }
  if(element->opening(YARS_STRING_FILTER))
  {
    _filter = new DataFilter(this);
    current = _filter;
    _filter->add(element);
  }
  if(element->opening(YARS_STRING_MAPPING))
  {
    DataDomainFactory::set(_mapping, element);
  }
}

DataGenericInternalEnergySensor* DataGenericInternalEnergySensor::_copy()
{
  DataGenericInternalEnergySensor *copy = new DataGenericInternalEnergySensor(NULL);

  copy->_name = _name;
  if (_noise) copy->_noise.reset(_noise->copy());
  if (_filter != NULL) copy->_filter = _filter->copy();
  copy->_mapping = _mapping;
  return copy;
}

void DataGenericInternalEnergySensor::_resetTo(const DataSensor *sensor)
{
  DataGenericInternalEnergySensor *other = (DataGenericInternalEnergySensor*)sensor;
  _name    = other->name();
  // deep copy: aliasing another sensor's noise would double-free
  _noise.reset(other->noise() ? other->noise()->copy() : nullptr);
  _filter  = other->filter();
  _mapping = other->mapping();
}
