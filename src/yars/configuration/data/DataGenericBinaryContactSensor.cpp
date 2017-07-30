#include <yars/configuration/data/DataGenericBinaryContactSensor.h>
#include <yars/configuration/data/DataDomainFactory.h>

#include <yars/defines/mutex.h>

# define YARS_STRING_MAPPING            (char*)"domain"
# define YARS_STRING_POSE               (char*)"pose"
# define YARS_STRING_NAME               (char*)"name"
# define YARS_STRING_OBJECT             (char*)"object"
# define YARS_STRING_DOMAIN             (char*)"domain"
# define YARS_STRING_MIN_MAX_DEFINITION (char*)"min"     DIVIDER "max" DIVIDER DEFINITION
# define YARS_STRING_NAME_DEFINITION    (char*)"name"          DIVIDER DEFINITION

DataGenericBinaryContactSensor::DataGenericBinaryContactSensor(DataNode* parent)
  : DataSensor(parent, DATA_GENERIC_BINARY_CONTACT_SENSOR)
{
  _domain.min = 0;
  _domain.max = 1;
  YM_INIT;
}

DataGenericBinaryContactSensor::~DataGenericBinaryContactSensor()
{
  YM_CLOSE;
}

void DataGenericBinaryContactSensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GENERIC_BINARY_CONTACT_SENSOR))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_GENERIC_BINARY_CONTACT_SENSOR))
  {
    element->set(YARS_STRING_NAME, _name);
  }
  if(element->opening(YARS_STRING_OBJECT))
  {
    element->set(YARS_STRING_NAME, _object);
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

void DataGenericBinaryContactSensor::createXsd(XsdSpecification *spec)
{
  XsdSequence *sensor = new XsdSequence(YARS_STRING_GENERIC_BINARY_CONTACT_SENSOR_DEFINITION);
  sensor->add(NA(YARS_STRING_NAME,   YARS_STRING_XSD_STRING,         false));
  sensor->add(NE(YARS_STRING_OBJECT, YARS_STRING_NAME_DEFINITION,    1, 1));
  sensor->add(NE(YARS_STRING_DOMAIN, YARS_STRING_MIN_MAX_DEFINITION, 0, 1));
  spec->add(sensor);
  DataNoise::createXsd(spec);
  DataFilter::createXsd(spec);
}

DataGenericBinaryContactSensor* DataGenericBinaryContactSensor::_copy()
{
  DataGenericBinaryContactSensor *copy = new DataGenericBinaryContactSensor(NULL);
  copy->_name = _name;
  copy->_object = _object;
  copy->_domain = _domain;
  if (_filter != NULL) copy->_filter = _filter->copy();
  if (_noise != NULL) copy->_noise = _noise->copy();
  return copy;
}

double DataGenericBinaryContactSensor::internalValue(int index)
{
  YM_LOCK;
  double r = _value;
  YM_UNLOCK;
  return r;
}

double DataGenericBinaryContactSensor::externalValue(int index)
{
  YM_LOCK;
  double r = _value;
  YM_UNLOCK;
  return r;
}

void DataGenericBinaryContactSensor::setInternalValue(int index, double v)
{
  YM_LOCK;
  if(v < 0.5) _value = _domain.min;
  else        _value = _domain.max;
  YM_UNLOCK;
}

void DataGenericBinaryContactSensor::setExternalValue(int index, double v)
{
  YM_LOCK;
  _value = v;
  YM_UNLOCK;
}

void DataGenericBinaryContactSensor::_resetTo(const DataSensor *sensor)
{
  DataGenericBinaryContactSensor *other = (DataGenericBinaryContactSensor*)sensor;
  _name    = other->_name;
  _object  = other->_object;
  _domain  = other->_domain;
}
