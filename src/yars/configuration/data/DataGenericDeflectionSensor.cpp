#include <yars/configuration/data/DataGenericDeflectionSensor.h>
#include <yars/configuration/data/DataDomainFactory.h>

#include <yars/util/noise/NoiseFactory.h>
#include <yars/defines/mutex.h>

# define YARS_STRING_MAPPING            (char*)"mapping"
# define YARS_STRING_POSE               (char*)"pose"
# define YARS_STRING_NAME               (char*)"name"
# define YARS_STRING_OBJECT             (char*)"object"
# define YARS_STRING_DOMAIN             (char*)"domain"
# define YARS_STRING_MIN_MAX_DEFINITION (char*)"min"     DIVIDER "max" DIVIDER DEFINITION
# define YARS_STRING_NAME_DEFINITION    (char*)"name"    DIVIDER DEFINITION


DataGenericDeflectionSensor::DataGenericDeflectionSensor(DataNode* parent)
  : DataSensor(parent, DATA_GENERIC_DEFLECTION_SENSOR)
{
  YM_INIT;
  _useRad = false;
}

DataGenericDeflectionSensor::~DataGenericDeflectionSensor()
{
  YM_CLOSE;
  delete _n;
}

void DataGenericDeflectionSensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GENERIC_DEFLECTION_SENSOR))
  {
    __setMapping();
    current = parent;
  }
  if(element->opening(YARS_STRING_GENERIC_DEFLECTION_SENSOR))
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
  if(element->opening(YARS_STRING_DOMAIN))
  {
    DataDomainFactory::set(_domain, element);
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

void DataGenericDeflectionSensor::createXsd(XsdSpecification *spec)
{
  XsdSequence *sensor = new XsdSequence(YARS_STRING_GENERIC_DEFLECTION_SENSOR_DEFINITION);
  sensor->add(NA(YARS_STRING_NAME,    YARS_STRING_XSD_STRING,         false));
  sensor->add(NE(YARS_STRING_OBJECT,  YARS_STRING_NAME_DEFINITION,    1, 1));
  sensor->add(NE(YARS_STRING_DOMAIN,  YARS_STRING_MIN_MAX_DEFINITION, 0, 1));
  sensor->add(NE(YARS_STRING_MAPPING, YARS_STRING_MIN_MAX_DEFINITION, 0, 1));
  sensor->add(NE(YARS_STRING_NOISE,   YARS_STRING_NOISE_DEFINITION,   0, 1));
  sensor->add(NE(YARS_STRING_FILTER,  YARS_STRING_FILTER_DEFINITION,  0, 1));
  spec->add(sensor);
  DataNoise::createXsd(spec);
  DataFilter::createXsd(spec);
}

Domain DataGenericDeflectionSensor::domain()
{
  YM_LOCK;
  Domain d = _domain;
  YM_UNLOCK;
  return d;
}

DataGenericDeflectionSensor* DataGenericDeflectionSensor::_copy()
{
  DataGenericDeflectionSensor *copy = new DataGenericDeflectionSensor(NULL);
  copy->_name = _name;
  copy->_object = _object;
  copy->_mapping = _mapping;
  copy->_domain = _domain;
  if (_filter != NULL) copy->_filter = _filter->copy();
  if (_noise != NULL) copy->_noise = _noise->copy();
  copy->__setMapping();
  copy->_useRad = _useRad;
  return copy;
}

double DataGenericDeflectionSensor::internalValue(int index)
{
  YM_LOCK;
  double r = _internalValue;
  YM_UNLOCK;
  return r;
}

double DataGenericDeflectionSensor::externalValue(int index)
{
  YM_LOCK;
  double r = _externalValue;
  YM_UNLOCK;
  return r;
}

void DataGenericDeflectionSensor::setInternalValue(int index, double v)
{
  YM_LOCK;
  _internalValue = _internalDomain.cut(v);
  _externalValue = _externalDomain.cut(_internalExternalMapping.map(_internalValue));
  YM_UNLOCK;
}

void DataGenericDeflectionSensor::setExternalValue(int index, double v)
{
  YM_LOCK;
  _externalValue = _externalDomain.cut(v);
  _internalValue = _internalExternalMapping.invMap(_externalValue);
  YM_UNLOCK;
}

void DataGenericDeflectionSensor::__setMapping()
{
  _externalDomain = _mapping;
  _internalDomain = _domain;
  _internalExternalMapping.setInputDomain(_internalDomain);
  _internalExternalMapping.setOutputDomain(_externalDomain);
  _n = NoiseFactory::create(_noise);

  if(_useRad)
  {
    _internalDomain *= M_PI/180.0;
    _internalExternalMapping.setInputDomain(_internalDomain);
  }
}

void DataGenericDeflectionSensor::setInternalDomain(Domain d)
{
  YM_LOCK;
  _internalDomain = d;
  __setMapping();
  YM_UNLOCK;
}

void DataGenericDeflectionSensor::setExternalDomain(Domain d)
{
  YM_LOCK;
  _externalDomain = d;
  __setMapping();
  YM_UNLOCK;
}

Domain DataGenericDeflectionSensor::getInternalDomain(int index)
{
  YM_LOCK;
  Domain r = _internalDomain;
  YM_UNLOCK;
  return r;
}

Domain DataGenericDeflectionSensor::getExternalDomain(int index)
{
  YM_LOCK;
  Domain r = _externalDomain;
  YM_UNLOCK;
  return r;
}

void DataGenericDeflectionSensor::_resetTo(const DataSensor *sensor)
{
  DataGenericDeflectionSensor *other = (DataGenericDeflectionSensor*)sensor;
  _name    = other->name();
  _object  = other->object();
  _mapping = other->mapping();
  _domain  = other->domain();
  _filter  = other->filter();
  _noise   = other->noise();
  __setMapping();
}

void DataGenericDeflectionSensor::setDegToRad()
{
  _useRad = true;
  __setMapping();
}
