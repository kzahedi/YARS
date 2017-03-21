#include "DataGenericCameraSensor.h"
#include "DataPoseFactory.h"
#include "DataDomainFactory.h"

# define YARS_STRING_OPENING_ANGLES     (char*)"openingAngles"
# define YARS_STRING_MAPPING            (char*)"mapping"
# define YARS_STRING_DIMENSION          (char*)"dimension"
# define YARS_STRING_POSE               (char*)"pose"
# define YARS_STRING_OBJECT             (char*)"object"
# define YARS_STRING_POSE_DEFINITION    (char*)"pose"          DIVIDER DEFINITION
# define YARS_STRING_MIN_MAX_DEFINITION (char*)"min"           DIVIDER "max"      DIVIDER DEFINITION
# define YARS_STRING_XY_DEFINITION      (char*)"xy"            DIVIDER DEFINITION


DataGenericCameraSensor::DataGenericCameraSensor(DataNode *parent)
  : DataSensor(parent, DATA_GENERIC_CAMERA_SENSOR)
{
  //resize(1);
}

DataGenericCameraSensor::~DataGenericCameraSensor()
{
}

void DataGenericCameraSensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GENERIC_CAMERA_SENSOR))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_GENERIC_CAMERA_SENSOR))
  {
    element->set(YARS_STRING_NAME, _name);
  }
  if(element->opening(YARS_STRING_OBJECT))
  {
    element->set(YARS_STRING_NAME, _object);
  }
  if(element->opening(YARS_STRING_POSE))
  {
    DataPoseFactory::set(_pose, element);
  }
  if(element->opening(YARS_STRING_MAPPING))
  {
    DataDomainFactory::set(_mapping, element);
  }
  if(element->opening(YARS_STRING_OPENING_ANGLES))
  {
    element->set(YARS_STRING_X, _openingAngles.x);
    element->set(YARS_STRING_Y, _openingAngles.y);
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

Pose DataGenericCameraSensor::pose()
{
  return _pose;
}

P2D DataGenericCameraSensor::openingAngles()
{
  return _openingAngles;
}

void DataGenericCameraSensor::createXsd(XsdSpecification *spec)
{
  XsdSequence *sensor = new XsdSequence(YARS_STRING_GENERIC_CAMERA_SENSOR_DEFINITION);
  sensor->add(NA(YARS_STRING_NAME,           YARS_STRING_XSD_STRING,         false));
  sensor->add(NE(YARS_STRING_OBJECT,         YARS_STRING_NAME_DEFINITION,    1, 1));
  sensor->add(NE(YARS_STRING_POSE,           YARS_STRING_POSE_DEFINITION,    1, 1));
  sensor->add(NE(YARS_STRING_OPENING_ANGLES, YARS_STRING_XY_DEFINITION,      1, 1));
  sensor->add(NE(YARS_STRING_MAPPING,        YARS_STRING_MIN_MAX_DEFINITION, 1, 1));
  sensor->add(NE(YARS_STRING_NOISE,          YARS_STRING_NOISE_DEFINITION,   0, 1));
  sensor->add(NE(YARS_STRING_FILTER,         YARS_STRING_FILTER_DEFINITION,  0, 1));
  spec->add(sensor);
  DataNoise::createXsd(spec);
  DataFilter::createXsd(spec);
}

DataGenericCameraSensor* DataGenericCameraSensor::_copy()
{
  DataGenericCameraSensor *copy = new DataGenericCameraSensor(NULL);
  copy->_name = _name;
  copy->_object = _object;
  copy->_pose = _pose;
  copy->_openingAngles = _openingAngles;
  copy->_mapping = _mapping;
  if (_noise != NULL) copy->_noise = _noise->copy();
  if (_filter != NULL) copy->_filter = _filter->copy();
  return copy;
}

void DataGenericCameraSensor::_resetTo(const DataSensor *sensor)
{
  DataGenericCameraSensor *other = (DataGenericCameraSensor*)sensor;
  _name          = other->name();
  _object        = other->object();
  _pose          = other->pose();
  _openingAngles = other->openingAngles();
  _mapping       = other->mapping();
  _filter        = other->filter();
  _noise         = other->noise();
}
