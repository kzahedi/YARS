#include "DataGenericAmbientLightSensor.h"
#include "DataDomainFactory.h"

#include <yars/defines/mutex.h>
#include <yars/util/noise/NoiseFactory.h>

# define YARS_STRING_MAPPING            (char*)"mapping"
# define YARS_STRING_POSE               (char*)"pose"
# define YARS_STRING_NAME               (char*)"name"
# define YARS_STRING_MIN_MAX_DEFINITION (char*)"min_max_definition"

DataGenericAmbientLightSensor::DataGenericAmbientLightSensor(DataNode* parent)
  : DataSensor(parent,DATA_GENERIC_AMBIENT_LIGHT_SENSOR)
{
  _noise = new DataNoise(this);
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
    _n = NoiseFactory::create(_noise);
  }
  if(element->opening(YARS_STRING_GENERIC_AMBIENT_LIGHT_SENSOR))
  {
    element->set(YARS_STRING_NAME, _name);
  }
  if(element->opening(YARS_STRING_NOISE))
  {
    current = _noise;
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

void DataGenericAmbientLightSensor::createXsd(XsdSpecification *spec)
{
  XsdSequence *sensor = new XsdSequence(YARS_STRING_GENERIC_AMBIENT_LIGHT_SENSOR_DEFINITION);
  sensor->add(NA(YARS_STRING_NAME,    YARS_STRING_XSD_STRING,         false));
  sensor->add(NE(YARS_STRING_MAPPING, YARS_STRING_MIN_MAX_DEFINITION, 1, 1));
  sensor->add(NE(YARS_STRING_NOISE,   YARS_STRING_NOISE_DEFINITION,   0, 1));
  sensor->add(NE(YARS_STRING_FILTER,  YARS_STRING_FILTER_DEFINITION,  0, 1));
  spec->add(sensor);
  DataNoise::createXsd(spec);
  DataFilter::createXsd(spec);
}


DataGenericAmbientLightSensor* DataGenericAmbientLightSensor::_copy()
{
  DataGenericAmbientLightSensor *copy = new DataGenericAmbientLightSensor(NULL);
  copy->_name = _name;
  if (_noise != NULL) copy->_noise = _noise->copy();
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
  _noise  = other->noise();
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
  _n = NoiseFactory::create(_noise);
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


