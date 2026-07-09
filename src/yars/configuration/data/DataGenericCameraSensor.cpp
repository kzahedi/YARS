#include "DataGenericCameraSensor.h"
#include "DataBinding.h"
#include "DataPoseFactory.h"
#include "DataDomainFactory.h"

# define YARS_STRING_OPENING_ANGLES     (char*)"openingAngles"
# define YARS_STRING_MAPPING            (char*)"mapping"
# define YARS_STRING_DIMENSION          (char*)"dimension"
# define YARS_STRING_POSE               (char*)"pose"
# define YARS_STRING_NAME               (char*)"name"
# define YARS_STRING_OBJECT             (char*)"object"
# define YARS_STRING_POSE_DEFINITION    (char*)"pose_definition"
# define YARS_STRING_MIN_MAX_DEFINITION (char*)"min_max_definition"
# define YARS_STRING_XY_DEFINITION      (char*)"xy_definition"
# define YARS_STRING_NAME_DEFINITION    (char*)"name_definition"


namespace
{
// Attribute binding table for the camera sensor's own opening tag.
// Child-element dispatch stays hand-written below.
const std::vector<yars::AttributeBinding> &genericCameraSensorAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_NAME,
       [](DataNode *self, const std::string &value)
       { static_cast<DataGenericCameraSensor *>(self)->setName(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

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
    yars::applyAttributes(this, element, genericCameraSensorAttributeBindings());
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
}

Pose DataGenericCameraSensor::pose()
{
  return _pose;
}

P2D DataGenericCameraSensor::openingAngles()
{
  return _openingAngles;
}

DataGenericCameraSensor* DataGenericCameraSensor::_copy()
{
  DataGenericCameraSensor *copy = new DataGenericCameraSensor(NULL);
  copy->_name = _name;
  copy->_object = _object;
  copy->_pose = _pose;
  copy->_openingAngles = _openingAngles;
  copy->_mapping = _mapping;
  if (_noise) copy->_noise.reset(_noise->copy());
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
  // deep copy: aliasing another sensor's noise would double-free
  _noise.reset(other->noise() ? other->noise()->copy() : nullptr);
}
