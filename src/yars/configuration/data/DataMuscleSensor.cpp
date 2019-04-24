#include "DataMuscleSensor.h"
#include "DataDomainFactory.h"

# define YARS_STRING_MAPPING            (char*)"mapping"
# define YARS_STRING_POSE               (char*)"pose"
# define YARS_STRING_NAME               (char*)"name"
# define YARS_STRING_OBJECT             (char*)"object"
# define YARS_STRING_DOMAIN             (char*)"domain"
# define YARS_STRING_MIN_MAX_DEFINITION (char*)"min_max_definition"
# define YARS_STRING_NAME_DEFINITION    (char*)"name_definition"


DataMuscleSensor::DataMuscleSensor(DataNode* parent)
  : DataSensor(parent, DATA_MUSCLE_SENSOR)
{
  _dimension = 5;
  _internalValue.resize(_dimension);
  _externalValue.resize(_dimension);
  _internalDomain.resize(_dimension);
  _externalDomain.resize(_dimension);
  _mapping.resize(_dimension);
  _domain.resize(_dimension);
  _internalExternalMapping.resize(_dimension);
  _dmIndex = 0;
 }

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
    DataDomainFactory::set(_mapping[_dmIndex], element);
    _dmIndex++; // mapping follows domain
  }
   if(element->opening(YARS_STRING_DOMAIN))
  {
    DataDomainFactory::set(_domain[_dmIndex], element);
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
  sensor->add(XE(YARS_STRING_OBJECT,  YARS_STRING_NAME_DEFINITION,    1, 1));
  for (int i = 0; i < 5; i++)
  {
    sensor->add(XE(YARS_STRING_DOMAIN, YARS_STRING_MIN_MAX_DEFINITION, 1, 1));
    sensor->add(XE(YARS_STRING_MAPPING, YARS_STRING_MIN_MAX_DEFINITION, 1, 1));
  }
  sensor->add(XE(YARS_STRING_NOISE, YARS_STRING_NOISE_DEFINITION, 0, 1));
  sensor->add(XE(YARS_STRING_FILTER,  YARS_STRING_FILTER_DEFINITION,  0, 1));
  spec->add(sensor);
  DataNoise::createXsd(spec);
  DataFilter::createXsd(spec);
}

DataMuscleSensor*  DataMuscleSensor::_copy()
{
  DataMuscleSensor *copy = new DataMuscleSensor(NULL);
  copy->_name = _name;
  copy->_object = _object;
  for(int i = 0; i < _dimension; i++)
  {
    copy->_mapping[i] = _mapping[i];
    copy->_domain[i] = _domain[i];
  }
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
  _internalValue[index] = _internalDomain[index].cut(v);
  _externalValue[index] = _internalExternalMapping[index].map(_internalValue[index]);
}

void DataMuscleSensor::setExternalValue(int index, double v)
{
  _externalValue[index] = v;
  _internalValue[index] = v;
}

void DataMuscleSensor::__setMapping()
{
 for(int i = 0; i < _dimension; i++)
  {
    _externalDomain[i] = _mapping[i];
    _internalDomain[i] = _domain[i];
    _internalExternalMapping[i].setInputDomain(_internalDomain[i]);
    _internalExternalMapping[i].setOutputDomain(_externalDomain[i]);
  }
}

Domain DataMuscleSensor::getInternalDomain(int index)
{
  return _internalDomain[index];
}

Domain DataMuscleSensor::getExternalDomain(int index)
{
  return _externalDomain[index];
}

void DataMuscleSensor::_resetTo(const DataSensor *sensor)
{
  DataMuscleSensor *other = (DataMuscleSensor*)sensor;
  _name    = other->name();
  _object  = other->object();
  for(int i = 0; i < _dimension; i++)
  {
    _mapping[i] = other->mapping(i);
    _domain[i] = other->domain(i);
  }
  _filter  = other->filter();
  _noise   = other->noise();
  __setMapping();
}

  Domain DataMuscleSensor::mapping(int i)
  {
    return _mapping[i];
  }

 Domain DataMuscleSensor::domain(int i)
 {
   return _domain[i];
 }