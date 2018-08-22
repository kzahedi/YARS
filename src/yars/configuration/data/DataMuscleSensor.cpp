#include "DataMuscleSensor.h"
#include "DataDomainFactory.h"

# define YARS_STRING_MAPPING            (char*)"mapping"
# define YARS_STRING_POSE               (char*)"pose"
# define YARS_STRING_NAME               (char*)"name"
# define YARS_STRING_OBJECT             (char*)"object"
# define YARS_STRING_MIN_MAX_DEFINITION (char*)"min_max_definition"
# define YARS_STRING_NAME_DEFINITION    (char*)"name_definition"


DataMuscleSensor::DataMuscleSensor(DataNode* parent)
  : DataSensor(parent, DATA_MUSCLE_SENSOR)
{ }

DataMuscleSensor::~DataMuscleSensor()
{
}

void DataMuscleSensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_MUSCLE_SENSOR))
  {
    __setMapping();
    current = parent;
  }
  if(element->opening(YARS_STRING_MUSCLE_SENSOR))
  {
    element->set(YARS_STRING_NAME, _name);
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

void DataMuscleSensor::createXsd(XsdSpecification *spec)
{
  XsdSequence *sensor = new XsdSequence(YARS_STRING_MUSCLE_SENSOR_DEFINITION);
  sensor->add(NA(YARS_STRING_NAME,    YARS_STRING_XSD_STRING,         false));
  sensor->add(NE(YARS_STRING_OBJECT,  YARS_STRING_NAME_DEFINITION,    1, 1));
  sensor->add(NE(YARS_STRING_MAPPING, YARS_STRING_MIN_MAX_DEFINITION, 1, 1));
  sensor->add(NE(YARS_STRING_NOISE,   YARS_STRING_NOISE_DEFINITION,   0, 1));
  sensor->add(NE(YARS_STRING_FILTER,  YARS_STRING_FILTER_DEFINITION,  0, 1));
  spec->add(sensor);
  DataNoise::createXsd(spec);
  DataFilter::createXsd(spec);
}

DataMuscleSensor*  DataMuscleSensor::_copy()
{
  DataMuscleSensor *copy = new DataMuscleSensor(NULL);
  copy->_name = _name;
  copy->_object = _object;
  copy->_mapping = _mapping;
  if (_filter != NULL) copy->_filter = _filter->copy();
  if (_noise != NULL) copy->_noise = _noise->copy();
  copy->__setMapping();
  return copy;
}

double DataMuscleSensor::internalValue(int index)
{
  return _internalValue[index];
}

double DataMuscleSensor::externalValue(int index)
{
  return _externalValue[index];
}

void DataMuscleSensor::setInternalValue(int index, double v)
{
  if(index == 0)
  {
    _internalValue[index] = _internalDomain.cut(v);
    _externalValue[index] = _internalExternalMapping.map(_internalValue[index]);
  }
  else
  {
    _internalValue[index] = v;
    _internalValue[index] = v;
  }
}

void DataMuscleSensor::setExternalValue(int index, double v)
{
  if(index == 0)
  {
  _externalValue[index] = _externalDomain.cut(v);
  _internalValue[index] = _internalExternalMapping.invMap(_externalValue[index]);
  }
  else
  {
    _externalValue[index] = v;
    _internalValue[index] = v;
  }
}

void DataMuscleSensor::__setMapping()
{
  _externalDomain = _mapping;
  _internalDomain = _domain;
  _internalExternalMapping.setInputDomain(_internalDomain);
  _internalExternalMapping.setOutputDomain(_externalDomain);
}

Domain DataMuscleSensor::getInternalDomain(int index)
{
  return _internalDomain;
}

Domain DataMuscleSensor::getExternalDomain(int index)
{
  return _externalDomain;
}

void DataMuscleSensor::_resetTo(const DataSensor *sensor)
{
  DataMuscleSensor *other = (DataMuscleSensor*)sensor;
  _name    = other->name();
  _object  = other->object();
  _mapping = other->mapping();
  _filter  = other->filter();
  _noise   = other->noise();
  __setMapping();
}