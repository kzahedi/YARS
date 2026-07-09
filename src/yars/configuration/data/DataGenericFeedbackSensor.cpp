#include "DataGenericFeedbackSensor.h"
#include "DataBinding.h"
#include "DataDomainFactory.h"

# define YARS_STRING_MAPPING            (char*)"mapping"
# define YARS_STRING_POSE               (char*)"pose"
# define YARS_STRING_NAME               (char*)"name"
# define YARS_STRING_OBJECT             (char*)"object"
# define YARS_STRING_MIN_MAX_DEFINITION (char*)"min_max_definition"
# define YARS_STRING_NAME_DEFINITION    (char*)"name_definition"


namespace
{
// Attribute binding table for the feedback sensor's own opening tag.
// Child-element dispatch stays hand-written below.
const std::vector<yars::AttributeBinding> &genericFeedbackSensorAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_NAME,
       [](DataNode *self, const std::string &value)
       { static_cast<DataGenericFeedbackSensor *>(self)->setName(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

DataGenericFeedbackSensor::DataGenericFeedbackSensor(DataNode* parent)
  : DataSensor(parent, DATA_GENERIC_FEEDBACK_SENSOR)
{ }

DataGenericFeedbackSensor::~DataGenericFeedbackSensor()
{
}

void DataGenericFeedbackSensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GENERIC_FEEDBACK_SENSOR))
  {
    __setMapping();
    current = parent;
  }
  if(element->opening(YARS_STRING_GENERIC_FEEDBACK_SENSOR))
  {
    yars::applyAttributes(this, element, genericFeedbackSensorAttributeBindings());
  }
  if(element->opening(YARS_STRING_OBJECT))
  {
    element->set(YARS_STRING_NAME, _object);
  }
  if(element->opening(YARS_STRING_MAPPING))
  {
    DataDomainFactory::set(_mapping, element);
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
}

DataGenericFeedbackSensor*  DataGenericFeedbackSensor::_copy()
{
  DataGenericFeedbackSensor *copy = new DataGenericFeedbackSensor(NULL);
  copy->_name = _name;
  copy->_object = _object;
  copy->_mapping = _mapping;
  if (_filter != NULL) copy->_filter = _filter->copy();
  if (_noise) copy->_noise.reset(_noise->copy());
  copy->__setMapping();
  return copy;
}

double DataGenericFeedbackSensor::internalValue(int index)
{
  return _internalValue;
}

double DataGenericFeedbackSensor::externalValue(int index)
{
  return _externalValue;
}

void DataGenericFeedbackSensor::setInternalValue(int index, double v)
{
  _internalValue = _internalDomain.cut(v);
  _externalValue = _internalExternalMapping.map(_internalValue);
}

void DataGenericFeedbackSensor::setExternalValue(int index, double v)
{
  _externalValue = _externalDomain.cut(v);
  _internalValue = _internalExternalMapping.invMap(_externalValue);
}

void DataGenericFeedbackSensor::__setMapping()
{
  _externalDomain = _mapping;
  _internalDomain = _domain;
  _internalExternalMapping.setInputDomain(_internalDomain);
  _internalExternalMapping.setOutputDomain(_externalDomain);
}

Domain DataGenericFeedbackSensor::getInternalDomain(int index)
{
  return _internalDomain;
}

Domain DataGenericFeedbackSensor::getExternalDomain(int index)
{
  return _externalDomain;
}

void DataGenericFeedbackSensor::_resetTo(const DataSensor *sensor)
{
  DataGenericFeedbackSensor *other = (DataGenericFeedbackSensor*)sensor;
  _name    = other->name();
  _object  = other->object();
  _mapping = other->mapping();
  _filter  = other->filter();
  // deep copy: aliasing another sensor's noise would double-free
  _noise.reset(other->noise() ? other->noise()->copy() : nullptr);
  __setMapping();
}
