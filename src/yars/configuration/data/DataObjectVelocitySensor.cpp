#include <yars/configuration/data/DataObjectVelocitySensor.h>
#include <yars/configuration/data/DataDomainFactory.h>

#include <yars/defines/mutex.h>

# define YARS_STRING_MAPPING                (char*)"mapping"
# define YARS_STRING_GLOBAL                 (char*)"global"
# define YARS_STRING_POSE                   (char*)"pose"
# define YARS_STRING_NAME                   (char*)"name"
# define YARS_STRING_OBJECT                 (char*)"object"
# define YARS_STRING_MIN_MAX_DEFINITION     (char*)"min_max_definition"
# define YARS_STRING_NAME_DEFINITION        (char*)"name_definition"

# define YARS_STRING_COORDINATES            (char*)"coordinates"
# define YARS_STRING_COORDINATES_DEFINITION (char*)"coordinates_definition"
# define YARS_STRING_TRUE_FALSE_DEFINITION  (char*)"true_false_definition"

# define YARS_STRING_X                      (char*)"x"
# define YARS_STRING_Y                      (char*)"y"
# define YARS_STRING_Z                      (char*)"z"

DataObjectVelocitySensor::DataObjectVelocitySensor(DataNode* parent)
  : DataSensor(parent, DATA_GENERIC_OBJECT_VELOCITY_SENSOR)
{
  _global = false;
  YM_INIT;
}

DataObjectVelocitySensor::~DataObjectVelocitySensor()
{
  YM_CLOSE;
  delete _value;
}


void DataObjectVelocitySensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GENERIC_OBJECT_VELOCITY_SENSOR))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_GENERIC_OBJECT_VELOCITY_SENSOR))
  {
    element->set(YARS_STRING_NAME,   _name);
    element->set(YARS_STRING_GLOBAL, _global);
  }
  if(element->opening(YARS_STRING_OBJECT))
  {
    element->set(YARS_STRING_NAME, _object);
  }
  if(element->opening(YARS_STRING_NOISE))
  {
    _noise  = std::make_unique<DataNoise>(this);
    current = _noise.get();
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

bool DataObjectVelocitySensor::x()
{
  YM_LOCK;
  bool r = _x;
  YM_UNLOCK;
  return r;
}

bool DataObjectVelocitySensor::y()
{
  YM_LOCK;
  bool r = _y;
  YM_UNLOCK;
  return r;
}

bool DataObjectVelocitySensor::z()
{
  YM_LOCK;
  bool r = _z;
  YM_UNLOCK;
  return r;
}

int DataObjectVelocitySensor::dimension()
{
  YM_LOCK;
  int r = _dimension;
  YM_UNLOCK;
  return r;
}

DataObjectVelocitySensor* DataObjectVelocitySensor::_copy()
{
  DataObjectVelocitySensor *copy = new DataObjectVelocitySensor(NULL);

  if (_noise) copy->_noise.reset(_noise->copy());
  if (_filter != NULL) copy->_filter = _filter->copy();
  copy->_name      = _name;
  copy->_object    = _object;
  copy->_mapping   = _mapping;
  copy->_x         = _x;
  copy->_y         = _y;
  copy->_z         = _z;
  copy->_dimension = _dimension;
  copy->_global    = _global;
  copy->_value     = new double[_dimension];
  return copy;
}

double DataObjectVelocitySensor::internalValue(int index)
{
  if (index < 0 || index >= _dimension) return 0.0;
  YM_LOCK;
  double r = _value[index];
  YM_UNLOCK;
  return r;
}

double DataObjectVelocitySensor::externalValue(int index)
{
  if (index < 0 || index >= _dimension) return 0.0;
  YM_LOCK;
  double r = _value[index];
  YM_UNLOCK;
  return r;
}

void DataObjectVelocitySensor::setInternalValue(int index, double v)
{
  YM_LOCK;
  _value[index] = v;
  YM_UNLOCK;
}

void DataObjectVelocitySensor::setExternalValue(int index, double v)
{
  YM_LOCK;
  _value[index] = v;
  YM_UNLOCK;
}

void DataObjectVelocitySensor::_resetTo(const DataSensor *sensor)
{
  DataObjectVelocitySensor *other = (DataObjectVelocitySensor*)sensor;

  _name      = other->name();
  _object    = other->object();
  _mapping   = other->mapping();
  // deep copy: aliasing another sensor's noise would double-free
  _noise.reset(other->noise() ? other->noise()->copy() : nullptr);
  _filter    = other->filter();
  _x         = other->x();
  _y         = other->y();
  _z         = other->z();
  _dimension = other->dimension();
  _global    = other->global();
}

bool DataObjectVelocitySensor::global()
{
  return _global;
}
