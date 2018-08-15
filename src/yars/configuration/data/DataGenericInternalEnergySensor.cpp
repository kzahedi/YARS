#include "DataGenericInternalEnergySensor.h"
#include "DataDomainFactory.h"

# define YARS_STRING_MAPPING            (char*)"mapping"
# define YARS_STRING_POSE               (char*)"pose"
# define YARS_STRING_NAME               (char*)"name"
# define YARS_STRING_MIN_MAX_DEFINITION (char*)"min_max_definition"


DataGenericInternalEnergySensor::DataGenericInternalEnergySensor(DataNode* parent)
  : DataSensor(parent, DATA_GENERIC_INTERNAL_ENERGY_SENSOR)
{ }

DataGenericInternalEnergySensor::~DataGenericInternalEnergySensor()
{
}

void DataGenericInternalEnergySensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GENERIC_INTERNAL_ENERGY_SENSOR))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_GENERIC_INTERNAL_ENERGY_SENSOR))
  {
    element->set(YARS_STRING_NAME, _name);
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
  if(element->opening(YARS_STRING_MAPPING))
  {
    DataDomainFactory::set(_mapping, element);
  }
}

void DataGenericInternalEnergySensor::createXsd(XsdSpecification *spec)
{
  XsdSequence *sensor = new XsdSequence(YARS_STRING_GENERIC_INTERNAL_ENERGY_SENSOR_DEFINITION);
  sensor->add(NA(YARS_STRING_NAME,    YARS_STRING_XSD_STRING,         false));
  sensor->add(NE(YARS_STRING_MAPPING, YARS_STRING_MIN_MAX_DEFINITION, 1, 1));
  sensor->add(NE(YARS_STRING_NOISE,   YARS_STRING_NOISE_DEFINITION,   0, 1));
  sensor->add(NE(YARS_STRING_FILTER,  YARS_STRING_FILTER_DEFINITION,  0, 1));
  spec->add(sensor);
  DataNoise::createXsd(spec);
  DataFilter::createXsd(spec);
}

DataGenericInternalEnergySensor* DataGenericInternalEnergySensor::_copy()
{
  DataGenericInternalEnergySensor *copy = new DataGenericInternalEnergySensor(NULL);

  copy->_name = _name;
  if (_noise != NULL) copy->_noise = _noise->copy();
  if (_filter != NULL) copy->_filter = _filter->copy();
  copy->_mapping = _mapping;
  return copy;
}

void DataGenericInternalEnergySensor::_resetTo(const DataSensor *sensor)
{
  DataGenericInternalEnergySensor *other = (DataGenericInternalEnergySensor*)sensor;
  _name    = other->name();
  _noise   = other->noise();
  _filter  = other->filter();
  _mapping = other->mapping();
}
