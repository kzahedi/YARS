#include "DataGenericLightDependentResistorSensor.h"
#include "DataBinding.h"
#include "DataPoseFactory.h"
#include "DataDomainFactory.h"
#include "DataColourFactory.h"

#include <yars/defines/mutex.h>
#include <yars/util/noise/NoiseFactory.h>

# define YARS_STRING_OPENING_ANGLES               (char*)"openingAngles"
# define YARS_STRING_OPENING                      (char*)"opening"
# define YARS_STRING_MAPPING                      (char*)"mapping"
# define YARS_STRING_POSE                         (char*)"pose"
# define YARS_STRING_NAME                         (char*)"name"
# define YARS_STRING_OBJECT                       (char*)"object"
# define YARS_STRING_POSE_DEFINITION              (char*)"pose_definition"
# define YARS_STRING_MIN_MAX_DEFINITION           (char*)"min_max_definition"
# define YARS_STRING_XY_DEFINITION                (char*)"xy_definition"
# define YARS_STRING_NAME_DEFINITION              (char*)"name_definition"
# define YARS_STRING_RAD                          (char*)"rad"
# define YARS_STRING_DEG                          (char*)"deg"

# define YARS_STRING_COLOR                        (char*)"colour"
# define YARS_STRING_COLOR_DEFINITION             (char*)"colour_rgb_definition"
# define YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION (char*)"colour_hex_rgb_definition"
# define YARS_STRING_XSD_HEX_COLOR                (char*)"[A-Fa-f0-9]{6}"

# define CONVERT_TO_RAD(x) x = x / 180.0 * M_PI
# define CONVERT_TO_DEG(x) x = x / M_PI  * 180.0

namespace
{
// Attribute binding table for the LDR sensor's own opening tag.
// Child-element dispatch stays hand-written below.
const std::vector<yars::AttributeBinding> &genericLightDependentResistorSensorAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_NAME,
       [](DataNode *self, const std::string &value)
       { static_cast<DataGenericLightDependentResistorSensor *>(self)->setName(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_OPENING,
       [](DataNode *self, const std::string &value)
       { static_cast<DataGenericLightDependentResistorSensor *>(self)->setOpening(atof(value.c_str())); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

DataGenericLightDependentResistorSensor::DataGenericLightDependentResistorSensor(DataNode* parent)
  : DataSensor(parent, DATA_GENERIC_LIGHT_DEPENDENT_RESISTOR_SENSOR)
{
  _noise = std::make_unique<DataNoise>(this);
  _n     = NULL;
}

DataGenericLightDependentResistorSensor::~DataGenericLightDependentResistorSensor()
{
  if(_n != NULL) delete _n;
}

void DataGenericLightDependentResistorSensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GENERIC_LDR_SENSOR))
  {
    current = parent;
    _n = NoiseFactory::create(_noise.get());
  }
  if(element->opening(YARS_STRING_GENERIC_LDR_SENSOR))
  {
    yars::applyAttributes(this, element, genericLightDependentResistorSensorAttributeBindings());
    CONVERT_TO_RAD(_openingAngle);
  }
  if(element->opening(YARS_STRING_OBJECT))
  {
    element->set(YARS_STRING_NAME, _object);
  }
  if(element->opening(YARS_STRING_POSE))
  {
    DataPoseFactory::set(_pose, element);
  }
  if(element->opening(YARS_STRING_COLOR))
  {
    string v;
    element->set(YARS_STRING_VALUE, v);
    DataColourFactory::set(_colour, v);
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

Pose DataGenericLightDependentResistorSensor::pose()
{
  return _pose;
}

double DataGenericLightDependentResistorSensor::opening()
{
  return _openingAngle;
}

void DataGenericLightDependentResistorSensor::setOpening(double opening)
{
  _openingAngle = opening;
}

Colour DataGenericLightDependentResistorSensor::colour()
{
  return _colour;
}

DataGenericLightDependentResistorSensor* DataGenericLightDependentResistorSensor::_copy()
{
  DataGenericLightDependentResistorSensor *copy = new DataGenericLightDependentResistorSensor(NULL);
  copy->_name          = _name;
  copy->_object        = _object;
  copy->_pose          = _pose;
  copy->_openingAngle  = _openingAngle;
  copy->_mapping       = _mapping;
  copy->_colour        = _colour;
  if (_noise) copy->_noise.reset(_noise->copy());
  if (_filter != NULL) copy->_filter = _filter->copy();
  copy->__setMapping();
  return copy;
}


void DataGenericLightDependentResistorSensor::_resetTo(const DataSensor *sensor)
{
  DataGenericLightDependentResistorSensor *other = (DataGenericLightDependentResistorSensor*)sensor;
  _name          = other->name();
  _object        = other->object();
  _pose          = other->pose();
  _openingAngle  = other->opening();
  // deep copy: aliasing another sensor's noise would double-free
  _noise.reset(other->noise() ? other->noise()->copy() : nullptr);
  _filter        = other->filter();
  _mapping       = other->mapping();
  _colour        = other->colour();
}

double DataGenericLightDependentResistorSensor::internalValue(int index)
{
  YM_LOCK;
  double r = _internalValue;
  YM_UNLOCK;
  return r;
}

double DataGenericLightDependentResistorSensor::externalValue(int index)
{
  YM_LOCK;
  double r = _externalValue;
  YM_UNLOCK;
  return r;
}

void DataGenericLightDependentResistorSensor::setInternalValue(int index, double v)
{
  YM_LOCK;
  _internalValue = _internalDomain.cut(v);
  _externalValue = _externalDomain.cut(_n->calculate(_internalExternalMapping.map(_internalValue)));
  YM_UNLOCK;
}

void DataGenericLightDependentResistorSensor::setExternalValue(int index, double v)
{
  YM_LOCK;
  _externalValue = _externalDomain.cut(v);
  _internalValue = _internalExternalMapping.invMap(_externalValue);
  YM_UNLOCK;
}

void DataGenericLightDependentResistorSensor::__setMapping()
{
  if(_n != NULL) delete _n;
  _externalDomain     = _mapping;
  _internalDomain.min = 0.0; _internalDomain.max = 1.0;
  _internalExternalMapping.setInputDomain(_internalDomain);
  _internalExternalMapping.setOutputDomain(_externalDomain);
  _n = NoiseFactory::create(_noise.get());
}

Domain DataGenericLightDependentResistorSensor::getInternalDomain(int index)
{
  YM_LOCK;
  Domain d = _internalDomain;
  YM_UNLOCK;
  return d;
}

Domain DataGenericLightDependentResistorSensor::getExternalDomain(int index)
{
  YM_LOCK;
  Domain d = _externalDomain;
  YM_UNLOCK;
  return d;
}


