#include "DataGenericSignalSensor.h"
#include "DataDomainFactory.h"

# define YARS_STRING_SOURCE             (char*)"source"
# define YARS_STRING_MAPPING            (char*)"mapping"
# define YARS_STRING_POSE               (char*)"pose"
# define YARS_STRING_NAME               (char*)"name"
# define YARS_STRING_OBJECT             (char*)"object"
# define YARS_STRING_MIN_MAX_DEFINITION (char*)"min_max_definition"
# define YARS_STRING_NAME_DEFINITION    (char*)"name_definition"

DataGenericSignalSensor::DataGenericSignalSensor(DataNode* parent)
  : DataSensor(parent, DATA_GENERIC_SIGNAL_SENSOR)
{
}

DataGenericSignalSensor::~DataGenericSignalSensor()
{
}

void DataGenericSignalSensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GENERIC_SIGNAL_SENSOR))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_GENERIC_SIGNAL_SENSOR))
  {
    element->set(YARS_STRING_NAME, _name);
  }
  if(element->opening(YARS_STRING_SOURCE))
  {
    element->set(YARS_STRING_NAME, _signal);
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

string DataGenericSignalSensor::signal()
{
  return _signal;
}

void DataGenericSignalSensor::createXsd(XsdSpecification *spec)
{
  XsdSequence *sensor = new XsdSequence(YARS_STRING_GENERIC_SIGNAL_SENSOR_DEFINITION);
  sensor->add(NA(YARS_STRING_NAME,    YARS_STRING_XSD_STRING,         false));
  sensor->add(NE(YARS_STRING_SOURCE,  YARS_STRING_NAME_DEFINITION,    1, 1));
  sensor->add(NE(YARS_STRING_MAPPING, YARS_STRING_MIN_MAX_DEFINITION, 1, 1));
  sensor->add(NE(YARS_STRING_NOISE,   YARS_STRING_NOISE_DEFINITION,   0, 1));
  sensor->add(NE(YARS_STRING_FILTER,  YARS_STRING_FILTER_DEFINITION,  0, 1));
  spec->add(sensor);
  DataNoise::createXsd(spec);
  DataFilter::createXsd(spec);
}

DataGenericSignalSensor* DataGenericSignalSensor::_copy()
{
  DataGenericSignalSensor *copy = new DataGenericSignalSensor(NULL);
  copy->_name = _name;
  copy->_signal = _signal;
  copy->_mapping = _mapping;
  if (_filter != NULL) copy->_filter = _filter->copy();
  if (_noise != NULL) copy->_noise = _noise->copy();
  return copy;
}

void DataGenericSignalSensor::_resetTo(const DataSensor *sensor)
{
  DataGenericSignalSensor *other = (DataGenericSignalSensor*)sensor;
  _name    = other->name();
  _signal  = other->signal();
  _mapping = other->mapping();
  _filter  = other->filter();
  _noise   = other->noise();
}
