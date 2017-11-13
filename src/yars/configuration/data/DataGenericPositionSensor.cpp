#include <yars/configuration/data/DataGenericPositionSensor.h>
#include <yars/configuration/data/DataDomainFactory.h>

#include <yars/defines/mutex.h>

# define YARS_STRING_MAPPING                (char*)"mapping"
# define YARS_STRING_POSE                   (char*)"pose"
# define YARS_STRING_OBJECT                 (char*)"object"
# define YARS_STRING_MIN_MAX_DEFINITION     (char*)"min"         DIVIDER "max" DIVIDER DEFINITION

# define YARS_STRING_COORDINATES            (char*)"coordinates"
# define YARS_STRING_COORDINATES_DEFINITION (char*)"coordinates" DIVIDER DEFINITION
# define YARS_STRING_TRUE_FALSE_DEFINITION  (char*)"true"        DIVIDER "false" DIVIDER DEFINITION

# define YARS_STRING_X                      (char*)"x"
# define YARS_STRING_Y                      (char*)"y"
# define YARS_STRING_Z                      (char*)"z"

DataGenericPositionSensor::DataGenericPositionSensor(DataNode* parent)
  : DataSensor(parent, DATA_GENERIC_POSITION_SENSOR)
{
  YM_INIT;
}

DataGenericPositionSensor::~DataGenericPositionSensor()
{
  YM_CLOSE;
  delete _value;
}


void DataGenericPositionSensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GENERIC_POSITION_SENSOR))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_GENERIC_POSITION_SENSOR))
  {
    element->set(YARS_STRING_NAME, _name);
  }
  if(element->opening(YARS_STRING_OBJECT))
  {
    element->set(YARS_STRING_NAME, _object);
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
  if(element->opening(YARS_STRING_COORDINATES))
  {
    element->set(YARS_STRING_X, _x);
    element->set(YARS_STRING_Y, _y);
    element->set(YARS_STRING_Z, _z);
    _dimension = 0;
    if(_x) _dimension++;
    if(_y) _dimension++;
    if(_z) _dimension++;
    _value = new double[_dimension];
  }
}

void DataGenericPositionSensor::createXsd(XsdSpecification *spec)
{
  XsdSequence *sensor = new XsdSequence(YARS_STRING_GENERIC_POSITION_SENSOR_DEFINITION);
  sensor->add(NA(YARS_STRING_NAME,        YARS_STRING_XSD_STRING,             false));
  sensor->add(NE(YARS_STRING_OBJECT,      YARS_STRING_NAME_DEFINITION,        1, 1));
  sensor->add(NE(YARS_STRING_COORDINATES, YARS_STRING_COORDINATES_DEFINITION, 1, 1));
  sensor->add(NE(YARS_STRING_NOISE,       YARS_STRING_NOISE_DEFINITION,       0, 1));
  sensor->add(NE(YARS_STRING_FILTER,      YARS_STRING_FILTER_DEFINITION,      0, 1));
  spec->add(sensor);

  XsdSequence *coordinates = new XsdSequence(YARS_STRING_COORDINATES_DEFINITION);
  coordinates->add(NA(YARS_STRING_X, YARS_STRING_TRUE_FALSE_DEFINITION, true));
  coordinates->add(NA(YARS_STRING_Y, YARS_STRING_TRUE_FALSE_DEFINITION, true));
  coordinates->add(NA(YARS_STRING_Z, YARS_STRING_TRUE_FALSE_DEFINITION, true));
  spec->add(coordinates);

  DataNoise::createXsd(spec);
  DataFilter::createXsd(spec);
}

bool DataGenericPositionSensor::x()
{
  YM_LOCK;
  bool r = _x;
  YM_UNLOCK;
  return r;
}

bool DataGenericPositionSensor::y()
{
  YM_LOCK;
  bool r = _y;
  YM_UNLOCK;
  return r;
}

bool DataGenericPositionSensor::z()
{
  YM_LOCK;
  bool r = _z;
  YM_UNLOCK;
  return r;
}

int DataGenericPositionSensor::dimension()
{
  YM_LOCK;
  int r = _dimension;
  YM_UNLOCK;
  return r;
}

DataGenericPositionSensor* DataGenericPositionSensor::_copy()
{
  DataGenericPositionSensor *copy = new DataGenericPositionSensor(NULL);

  copy->_name = _name;
  copy->_object = _object;
  copy->_mapping = _mapping;
  if (_noise != NULL) copy->_noise = _noise->copy();
  if (_filter != NULL) copy->_filter = _filter->copy();
  copy->_x = _x;
  copy->_y = _y;
  copy->_z = _z;
  copy->_dimension = _dimension;
  copy->_value = new double[_dimension];
  return copy;
}

double DataGenericPositionSensor::internalValue(int index)
{
  YM_LOCK;
  double r = _value[index];
  YM_UNLOCK;
  return r;
}

double DataGenericPositionSensor::externalValue(int index)
{
  YM_LOCK;
  double r = _value[index];
  YM_UNLOCK;
  return r;
}

void DataGenericPositionSensor::setInternalValue(int index, double v)
{
  YM_LOCK;
  _value[index] = v;
  YM_UNLOCK;
}

void DataGenericPositionSensor::setExternalValue(int index, double v)
{
  YM_LOCK;
  _value[index] = v;
  YM_UNLOCK;
}

void DataGenericPositionSensor::_resetTo(const DataSensor *sensor)
{
  DataGenericPositionSensor *other = (DataGenericPositionSensor*)sensor;

  _name      = other->name();
  _object    = other->object();
  _mapping   = other->mapping();
  _noise     = other->noise();
  _filter    = other->filter();
  _x         = other->x();
  _y         = other->y();
  _z         = other->z();
  _dimension = other->dimension();
}
