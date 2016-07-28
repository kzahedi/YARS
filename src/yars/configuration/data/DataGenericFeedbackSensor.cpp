#include "DataGenericFeedbackSensor.h"
#include "DataDomainFactory.h"

# define YARS_STRING_MAPPING            (char*)"mapping"
# define YARS_STRING_POSE               (char*)"pose"
# define YARS_STRING_NAME               (char*)"name"
# define YARS_STRING_OBJECT             (char*)"object"
# define YARS_STRING_MIN_MAX_DEFINITION (char*)"min"     DIVIDER "max" DIVIDER DEFINITION
# define YARS_STRING_NAME_DEFINITION    (char*)"name"          DIVIDER DEFINITION


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

void DataGenericFeedbackSensor::createXsd(XsdSpecification *spec)
{
  XsdSequence *sensor = new XsdSequence(YARS_STRING_GENERIC_FEEDBACK_SENSOR_DEFINITION);
  sensor->add(NA(YARS_STRING_NAME,    YARS_STRING_XSD_STRING,         false));
  sensor->add(NE(YARS_STRING_OBJECT,  YARS_STRING_NAME_DEFINITION,    1, 1));
  sensor->add(NE(YARS_STRING_MAPPING, YARS_STRING_MIN_MAX_DEFINITION, 1, 1));
  sensor->add(NE(YARS_STRING_NOISE,   YARS_STRING_NOISE_DEFINITION,   0, 1));
  sensor->add(NE(YARS_STRING_FILTER,  YARS_STRING_FILTER_DEFINITION,  0, 1));
  spec->add(sensor);
  DataNoise::createXsd(spec);
  DataFilter::createXsd(spec);
}

DataGenericFeedbackSensor*  DataGenericFeedbackSensor::_copy()
{
  DataGenericFeedbackSensor *copy = new DataGenericFeedbackSensor(NULL);
  copy->_name = _name;
  copy->_object = _object;
  copy->_mapping = _mapping;
  if (_filter != NULL) copy->_filter = _filter->copy();
  if (_noise != NULL) copy->_noise = _noise->copy();
  copy->__setMapping();
  return copy;
}

yReal DataGenericFeedbackSensor::internalValue(int index)
{
  return _internalValue;
}

yReal DataGenericFeedbackSensor::externalValue(int index)
{
  return _externalValue;
}

void DataGenericFeedbackSensor::setInternalValue(int index, yReal v)
{
  _internalValue = _internalDomain.cut(v);
  _externalValue = _internalExternalMapping.map(_internalValue);
}

void DataGenericFeedbackSensor::setExternalValue(int index, yReal v)
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
  _noise   = other->noise();
  __setMapping();
}
