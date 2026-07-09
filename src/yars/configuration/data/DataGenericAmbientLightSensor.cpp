#include "DataGenericAmbientLightSensor.h"
#include "DataBinding.h"
#include "DataDomainFactory.h"

#include <yars/defines/mutex.h>
#include <yars/util/noise/NoiseFactory.h>

# define YARS_STRING_MAPPING            (char*)"mapping"
# define YARS_STRING_POSE               (char*)"pose"
# define YARS_STRING_NAME               (char*)"name"
# define YARS_STRING_MIN_MAX_DEFINITION (char*)"min_max_definition"

namespace
{
// Attribute binding table for the ambient light sensor's own opening tag.
// Child-element dispatch stays hand-written below.
const std::vector<yars::AttributeBinding> &genericAmbientLightSensorAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_NAME,
       [](DataNode *self, const std::string &value)
       { static_cast<DataGenericAmbientLightSensor *>(self)->setName(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

DataGenericAmbientLightSensor::DataGenericAmbientLightSensor(DataNode* parent)
  : DataSensor(parent,DATA_GENERIC_AMBIENT_LIGHT_SENSOR)
{
  _noise = std::make_unique<DataNoise>(this);
  _n     = NULL;
}

DataGenericAmbientLightSensor::~DataGenericAmbientLightSensor()
{
}

void DataGenericAmbientLightSensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GENERIC_AMBIENT_LIGHT_SENSOR))
  {
    current = parent;
    _n = NoiseFactory::create(_noise.get());
  }
  if(element->opening(YARS_STRING_GENERIC_AMBIENT_LIGHT_SENSOR))
  {
    yars::applyAttributes(this, element, genericAmbientLightSensorAttributeBindings());
  }
  if(element->opening(YARS_STRING_NOISE))
  {
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

DataGenericAmbientLightSensor* DataGenericAmbientLightSensor::_copy()
{
  DataGenericAmbientLightSensor *copy = new DataGenericAmbientLightSensor(NULL);
  copy->_name = _name;
  if (_noise) copy->_noise.reset(_noise->copy());
  if (_filter != NULL) copy->_filter = _filter->copy();
  copy->_mapping = _mapping;
  copy->__setMapping();
  return copy;
}

void DataGenericAmbientLightSensor::_resetTo(const DataSensor *sensor)
{
  DataGenericAmbientLightSensor* other = (DataGenericAmbientLightSensor*)sensor;
  _name    = other->name();
  _mapping = other->mapping();
  // deep copy: aliasing another sensor's noise would double-free
  _noise.reset(other->noise() ? other->noise()->copy() : nullptr);
  _filter = other->filter();
}

double DataGenericAmbientLightSensor::internalValue(int index)
{
  YM_LOCK;
  double r = _internalValue;
  YM_UNLOCK;
  return r;
}

double DataGenericAmbientLightSensor::externalValue(int index)
{
  YM_LOCK;
  double r = _externalValue;
  YM_UNLOCK;
  return r;
}

void DataGenericAmbientLightSensor::setInternalValue(int index, double v)
{
  YM_LOCK;
  _internalValue = _internalDomain.cut(v);
  _externalValue = _externalDomain.cut(_n->calculate(_internalExternalMapping.map(_internalValue)));
  YM_UNLOCK;
}

void DataGenericAmbientLightSensor::setExternalValue(int index, double v)
{
  YM_LOCK;
  _externalValue = _externalDomain.cut(v);
  _internalValue = _internalExternalMapping.invMap(_externalValue);
  YM_UNLOCK;
}

void DataGenericAmbientLightSensor::__setMapping()
{
  if(_n != NULL) delete _n;
  _externalDomain     = _mapping;
  _internalDomain.min = 0.0; _internalDomain.max = 1.0;
  _internalExternalMapping.setInputDomain(_internalDomain);
  _internalExternalMapping.setOutputDomain(_externalDomain);
  _n = NoiseFactory::create(_noise.get());
}

Domain DataGenericAmbientLightSensor::getInternalDomain(int index)
{
  YM_LOCK;
  Domain d = _internalDomain;
  YM_UNLOCK;
  return d;
}

Domain DataGenericAmbientLightSensor::getExternalDomain(int index)
{
  YM_LOCK;
  Domain d = _externalDomain;
  YM_UNLOCK;
  return d;
}


