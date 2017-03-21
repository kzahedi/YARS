#include "DataGenericLightDependentResistorSensor.h"
#include "DataPoseFactory.h"
#include "DataDomainFactory.h"
#include "DataColourFactory.h"

#include <yars/defines/mutex.h>
#include <yars/util/noise/NoiseFactory.h>

# define YARS_STRING_OPENING_ANGLES               (char*)"openingAngles"
# define YARS_STRING_OPENING                      (char*)"opening"
# define YARS_STRING_MAPPING                      (char*)"mapping"
# define YARS_STRING_POSE                         (char*)"pose"
# define YARS_STRING_OBJECT                       (char*)"object"
# define YARS_STRING_POSE_DEFINITION              (char*)"pose" DIVIDER DEFINITION
# define YARS_STRING_MIN_MAX_DEFINITION           (char*)"min"  DIVIDER "max" DIVIDER DEFINITION
# define YARS_STRING_XY_DEFINITION                (char*)"xy"   DIVIDER DEFINITION
# define YARS_STRING_RAD                          (char*)"rad"
# define YARS_STRING_DEG                          (char*)"deg"

# define YARS_STRING_COLOR                        (char*)"colour"
# define YARS_STRING_COLOR_DEFINITION             (char*)"colour_rgb_definition"
# define YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION (char*)"colour_hex_rgb_definition"
# define YARS_STRING_XSD_HEX_COLOR                (char*)"[A-Fa-f0-9]{6}"

# define CONVERT_TO_RAD(x) x = x / 180.0 * M_PI
# define CONVERT_TO_DEG(x) x = x / M_PI  * 180.0

DataGenericLightDependentResistorSensor::DataGenericLightDependentResistorSensor(DataNode* parent)
  : DataSensor(parent, DATA_GENERIC_LIGHT_DEPENDENT_RESISTOR_SENSOR)
{
  _noise = new DataNoise(this);
  _n     = NULL;
}

DataGenericLightDependentResistorSensor::~DataGenericLightDependentResistorSensor()
{
  if(_n != NULL) delete _n;
  delete _noise;
}

void DataGenericLightDependentResistorSensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GENERIC_LDR_SENSOR))
  {
    current = parent;
    _n = NoiseFactory::create(_noise);
  }
  if(element->opening(YARS_STRING_GENERIC_LDR_SENSOR))
  {
    element->set(YARS_STRING_NAME, _name);
    element->set(YARS_STRING_OPENING, _openingAngle);
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
    _noise  = new DataNoise(this);
    current = _noise;
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

yReal DataGenericLightDependentResistorSensor::opening()
{
  return _openingAngle;
}

Colour DataGenericLightDependentResistorSensor::colour()
{
  return _colour;
}

void DataGenericLightDependentResistorSensor::createXsd(XsdSpecification *spec)
{
  XsdSequence *sensor = new XsdSequence(YARS_STRING_GENERIC_LDR_SENSOR_DEFINITION);
  sensor->add(NA(YARS_STRING_NAME,    YARS_STRING_XSD_STRING,                true));
  sensor->add(NA(YARS_STRING_OPENING, YARS_STRING_POSITIVE_NON_ZERO_DECIMAL, true));
  sensor->add(NE(YARS_STRING_OBJECT,  YARS_STRING_NAME_DEFINITION,           1, 1));
  sensor->add(NE(YARS_STRING_POSE,    YARS_STRING_POSE_DEFINITION,           1, 1));
  sensor->add(NE(YARS_STRING_COLOR,   YARS_STRING_COLOR_DEFINITION,          1, 1));
  sensor->add(NE(YARS_STRING_MAPPING, YARS_STRING_MIN_MAX_DEFINITION,        1, 1));
  sensor->add(NE(YARS_STRING_NOISE,   YARS_STRING_NOISE_DEFINITION,          0, 1));
  sensor->add(NE(YARS_STRING_FILTER,  YARS_STRING_FILTER_DEFINITION,         0, 1));
  spec->add(sensor);
  DataNoise::createXsd(spec);
  DataFilter::createXsd(spec);

  XsdSequence *color = new XsdSequence(YARS_STRING_COLOR_DEFINITION);
  color->add(NA(YARS_STRING_VALUE, YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION, true));
  spec->add(color);

  XsdRegularExpression *colorHex =
    new XsdRegularExpression(YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION,
                             YARS_STRING_XSD_STRING, YARS_STRING_XSD_HEX_COLOR);
  spec->add(colorHex);
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
  if (_noise  != NULL) copy->_noise  = _noise->copy();
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
  _noise         = other->noise();
  _filter        = other->filter();
  _mapping       = other->mapping();
  _colour        = other->colour();
}

yReal DataGenericLightDependentResistorSensor::internalValue(int index)
{
  YM_LOCK;
  yReal r = _internalValue;
  YM_UNLOCK;
  return r;
}

yReal DataGenericLightDependentResistorSensor::externalValue(int index)
{
  YM_LOCK;
  yReal r = _externalValue;
  YM_UNLOCK;
  return r;
}

void DataGenericLightDependentResistorSensor::setInternalValue(int index, yReal v)
{
  YM_LOCK;
  _internalValue = _internalDomain.cut(v);
  _externalValue = _externalDomain.cut(_n->calculate(_internalExternalMapping.map(_internalValue)));
  YM_UNLOCK;
}

void DataGenericLightDependentResistorSensor::setExternalValue(int index, yReal v)
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
  _n = NoiseFactory::create(_noise);
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


