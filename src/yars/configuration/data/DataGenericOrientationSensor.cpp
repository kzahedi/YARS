#include <yars/configuration/data/DataGenericOrientationSensor.h>
#include <yars/configuration/data/DataDomainFactory.h>

#include <yars/defines/mutex.h>

# define YARS_STRING_MAPPING                (char*)"mapping"
# define YARS_STRING_POSE                   (char*)"pose"
# define YARS_STRING_NAME                   (char*)"name"
# define YARS_STRING_OBJECT                 (char*)"object"
# define YARS_STRING_MIN_MAX_DEFINITION     (char*)"min"         DIVIDER "max"      DIVIDER DEFINITION
# define YARS_STRING_NAME_DEFINITION        (char*)"name"        DIVIDER DEFINITION

# define YARS_STRING_COORDINATES            (char*)"coordinates"
# define YARS_STRING_COORDINATES_DEFINITION (char*)"coordinates" DIVIDER DEFINITION
# define YARS_STRING_TRUE_FALSE_DEFINITION  (char*)"true"        DIVIDER "false"    DIVIDER DEFINITION

# define YARS_STRING_X                      (char*)"x"
# define YARS_STRING_Y                      (char*)"y"
# define YARS_STRING_Z                      (char*)"z"

# define YARS_STRING_RAD_DEG_DEFINITION     (char*)"radOrDeg"    DIVIDER DEFINITION
# define YARS_STRING_RAD                    (char*)"rad"
# define YARS_STRING_DEG                    (char*)"deg"

# define YARS_STRING_TYPE                   (char*)"type"

DataGenericOrientationSensor::DataGenericOrientationSensor(DataNode* parent)
  : DataSensor(parent, DATA_GENERIC_ORIENTATION_SENSOR)
{
  _useDegree = true;
  YM_INIT;
}

DataGenericOrientationSensor::~DataGenericOrientationSensor()
{
  YM_CLOSE;
  delete _value;
}


void DataGenericOrientationSensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GENERIC_ORIENTATION_SENSOR))
  {
    _value  = new yReal[_dimension];
    current = parent;
  }
  if(element->opening(YARS_STRING_GENERIC_ORIENTATION_SENSOR))
  {
    element->set(YARS_STRING_NAME, _name);
    if(element->attribute(YARS_STRING_TYPE) != NULL)
    {
      if(element->attribute(YARS_STRING_TYPE)->value() == YARS_STRING_DEG)
      {
        _useDegree = true;
      }
      else
      {
        _useDegree = false;
      }
    }
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
  }
}

bool DataGenericOrientationSensor::x()
{
  YM_LOCK;
  bool r = _x;
  YM_UNLOCK;
  return r;
}

bool DataGenericOrientationSensor::y()
{
  YM_LOCK;
  bool r = _y;
  YM_UNLOCK;
  return r;
}

bool DataGenericOrientationSensor::z()
{
  YM_LOCK;
  bool r = _z;
  YM_UNLOCK;
  return r;
}

int DataGenericOrientationSensor::dimension()
{
  YM_LOCK;
  int r = _dimension;
  YM_UNLOCK;
  return r;
}

void DataGenericOrientationSensor::createXsd(XsdSpecification *spec)
{
  XsdSequence *sensor = new XsdSequence(YARS_STRING_GENERIC_ORIENTATION_SENSOR_DEFINITION);
  sensor->add(NA(YARS_STRING_NAME,        YARS_STRING_XSD_STRING,             false));
  sensor->add(NA(YARS_STRING_TYPE, YARS_STRING_RAD_DEG_DEFINITION,            false));
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

bool DataGenericOrientationSensor::useDegree()
{
  YM_LOCK;
  bool r = _useDegree;
  YM_UNLOCK;
  return r;
}

DataGenericOrientationSensor* DataGenericOrientationSensor::_copy()
{
  DataGenericOrientationSensor *copy = new DataGenericOrientationSensor(NULL);
  copy->_name = _name;
  copy->_object = _object;
  copy->_mapping = _mapping;
  if (_noise != NULL) copy->_noise = _noise->copy();
  if (_filter != NULL) copy->_filter = _filter->copy();
  copy->_x = _x;
  copy->_y = _y;
  copy->_z = _z;
  copy->_dimension = _dimension;
  copy->_useDegree = _useDegree;
  copy->_value = new yReal[_dimension];
  return copy;
}

yReal DataGenericOrientationSensor::internalValue(int index)
{
  YM_LOCK;
  yReal r = _value[index];
  YM_UNLOCK;
  return r;
}

yReal DataGenericOrientationSensor::externalValue(int index)
{
  YM_LOCK;
  yReal r = _value[index];
  YM_UNLOCK;
  return r;
}

void DataGenericOrientationSensor::setInternalValue(int index, yReal v)
{
  YM_LOCK;
  _value[index] = v;
  YM_UNLOCK;
}

void DataGenericOrientationSensor::setExternalValue(int index, yReal v)
{
  YM_LOCK;
  _value[index] = v;
  YM_UNLOCK;
}

void DataGenericOrientationSensor::_resetTo(const DataSensor *sensor)
{
  DataGenericOrientationSensor *other = (DataGenericOrientationSensor*)sensor;
  _name = other->name();
  _object = other->object();
  _mapping = other->mapping();
  _noise = other->noise();
  _filter = other->filter();
  _x = other->x();
  _y = other->y();
  _z = other->z();
  _dimension = other->dimension();
  _useDegree = other->useDegree();
}
