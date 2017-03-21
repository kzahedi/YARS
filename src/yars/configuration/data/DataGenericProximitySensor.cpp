#include <yars/configuration/data/DataGenericProximitySensor.h>
#include <yars/configuration/data/DataPoseFactory.h>
#include <yars/configuration/data/DataDomainFactory.h>
#include <yars/util/noise/NoiseFactory.h>

#include <yars/defines/mutex.h>

#include <yars/configuration/xsd/specification/XsdSpecification.h>

# define YARS_STRING_DISTANCE                 (char*)"distance"
# define YARS_STRING_OPENING_ANGLES           (char*)"openingAngles"
# define YARS_STRING_MAPPING                  (char*)"mapping"
# define YARS_STRING_POSE                     (char*)"pose"
# define YARS_STRING_OBJECT                   (char*)"object"
# define YARS_STRING_POSE_DEFINITION          (char*)"pose"          DIVIDER DEFINITION
# define YARS_STRING_MIN_MAX_DEFINITION       (char*)"min_max"       DIVIDER DEFINITION
# define YARS_STRING_XY_DEFINITION            (char*)"xy"            DIVIDER DEFINITION
# define YARS_STRING_RAD_DEG_DEFINITION       (char*)"radOrDeg"      DIVIDER DEFINITION
# define YARS_STRING_RAD                      (char*)"rad"
# define YARS_STRING_DEG                      (char*)"deg"
# define YARS_STRING_METER                    (char*)"meter"
# define YARS_STRING_METER_DEFINITION         (char*)"meter"         DIVIDER DEFINITION
# define YARS_STRING_OPENING_ANGLES_DEFINTION (char*)"openingAngles" DIVIDER DEFINITION
# define YARS_STRING_POSITIVE_NON_ZERO_DECIMAL     (char*)"positive_non_zero_decimal" DIVIDER DEFINITION


# define CONVERT_TO_RAD(x) x = x / 180.0 * M_PI
# define CONVERT_TO_DEG(x) x = x / M_PI  * 180.0

DataGenericProximitySensor::DataGenericProximitySensor(DataNode *parent)
  : DataSensor(parent, DATA_GENERIC_PROXIMITY_SENSOR)
{
  _noise  = new DataNoise(this);
  _n = NULL;
  _measuredDistance = 0;
  YM_INIT;
}

DataGenericProximitySensor::~DataGenericProximitySensor()
{
  YM_CLOSE;
  delete _n;
}

void DataGenericProximitySensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GENERIC_PROXIMITY))
  {
    __setMapping();
    current = parent;
  }
  if(element->opening(YARS_STRING_GENERIC_PROXIMITY))
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
  if(element->opening(YARS_STRING_DISTANCE))
  {
    element->set(YARS_STRING_METER, _distance);
    _measuredDistance = _distance;
  }
  if(element->opening(YARS_STRING_MAPPING))
  {
    DataDomainFactory::set(_mapping, element);
  }
  if(element->opening(YARS_STRING_OPENING_ANGLES))
  {
    element->set(YARS_STRING_X, _openingAngles.x);
    element->set(YARS_STRING_Y, _openingAngles.y);
    CONVERT_TO_RAD(_openingAngles.x);
    CONVERT_TO_RAD(_openingAngles.y);
    if(element->attribute(YARS_STRING_TYPE) != NULL)
    {
      if(element->attribute(YARS_STRING_TYPE)->value() == YARS_STRING_RAD)
      {
        CONVERT_TO_DEG(_openingAngles.x);
        CONVERT_TO_DEG(_openingAngles.y);
      }
    }
  }
  if(element->opening(YARS_STRING_NOISE))
  {
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

Pose DataGenericProximitySensor::pose()
{
  YM_LOCK;
  Pose r = _pose;
  YM_UNLOCK;
  return r;
}

yReal DataGenericProximitySensor::distance()
{
  YM_LOCK;
  yReal r = _distance;
  YM_UNLOCK;
  return r;
}

P2D DataGenericProximitySensor::openingAngles()
{
  YM_LOCK;
  P2D r = _openingAngles;
  YM_UNLOCK;
  return r;
}

void DataGenericProximitySensor::createXsd(XsdSpecification *spec)
{
  XsdSequence *sensor = new XsdSequence(YARS_STRING_GENERIC_PROXIMITY_DEFINITION);
  sensor->add(NA(YARS_STRING_NAME,           YARS_STRING_XSD_STRING,               false));
  sensor->add(NE(YARS_STRING_OBJECT,         YARS_STRING_NAME_DEFINITION,          1, 1));
  sensor->add(NE(YARS_STRING_POSE,           YARS_STRING_POSE_DEFINITION,          1, 1));
  sensor->add(NE(YARS_STRING_DISTANCE,       YARS_STRING_METER_DEFINITION,         1, 1));
  sensor->add(NE(YARS_STRING_MAPPING,        YARS_STRING_MIN_MAX_DEFINITION,       1, 1));
  sensor->add(NE(YARS_STRING_OPENING_ANGLES, YARS_STRING_OPENING_ANGLES_DEFINTION, 1, 1));
  sensor->add(NE(YARS_STRING_NOISE,          YARS_STRING_NOISE_DEFINITION,         0, 1));
  sensor->add(NE(YARS_STRING_FILTER,         YARS_STRING_FILTER_DEFINITION,        0, 1));
  spec->add(sensor);

  XsdSequence *oaDefinition = new XsdSequence(YARS_STRING_OPENING_ANGLES_DEFINTION);
  oaDefinition->add(NA(YARS_STRING_X,    YARS_STRING_XSD_DECIMAL,        true));
  oaDefinition->add(NA(YARS_STRING_Y,    YARS_STRING_XSD_DECIMAL,        true));
  oaDefinition->add(NA(YARS_STRING_TYPE, YARS_STRING_RAD_DEG_DEFINITION, false));
  spec->add(oaDefinition);

  XsdSequence *meterDefinition = new XsdSequence(YARS_STRING_METER_DEFINITION);
  meterDefinition->add(NA(YARS_STRING_METER, YARS_STRING_POSITIVE_NON_ZERO_DECIMAL, true));
  spec->add(meterDefinition);

  DataNoise::createXsd(spec);
  DataFilter::createXsd(spec);
}

DataGenericProximitySensor* DataGenericProximitySensor::_copy()
{
  DataGenericProximitySensor *copy = new DataGenericProximitySensor(NULL);
  copy->_name             = _name;
  copy->_object           = _object;
  copy->_pose             = _pose;
  copy->_distance         = _distance;
  copy->_mapping          = _mapping;
  copy->_openingAngles    = _openingAngles;
  copy->_measuredDistance = _measuredDistance;
  if (_noise  != NULL) copy->_noise  = _noise->copy();
  if (_filter != NULL) copy->_filter = _filter->copy();
  copy->__setMapping();
  return copy;
}

void DataGenericProximitySensor::setMeasuredDistance(yReal dist)
{
  YM_LOCK;
  _measuredDistance = MIN(_distance, dist);
  YM_UNLOCK;
}

yReal DataGenericProximitySensor::getMeasuredDistance()
{
  YM_LOCK;
  yReal r = _measuredDistance;
  YM_UNLOCK;
  return r;
}

yReal DataGenericProximitySensor::internalValue(int index)
{
  YM_LOCK;
  yReal r = _internalValue;
  YM_UNLOCK;
  return r;
}

yReal DataGenericProximitySensor::externalValue(int index)
{
  YM_LOCK;
  yReal r = _externalValue;
  YM_UNLOCK;
  return r;
}

void DataGenericProximitySensor::setInternalValue(int index, yReal v)
{
  YM_LOCK;
  _internalValue = _internalDomain.cut(v);
  _externalValue = _externalDomain.cut(_n->calculate(_internalExternalMapping.map(_internalValue)));
  YM_UNLOCK;
}

void DataGenericProximitySensor::setExternalValue(int index, yReal v)
{
  YM_LOCK;
  _externalValue = _externalDomain.cut(v);
  _internalValue = _internalExternalMapping.invMap(_externalValue);
  YM_UNLOCK;
}

void DataGenericProximitySensor::__setMapping()
{
  if(_n != NULL) delete _n;
  _externalDomain     = _mapping;
  _internalDomain.min = 0; _internalDomain.max = _distance;
  _internalExternalMapping.setInputDomain(_internalDomain);
  _internalExternalMapping.setOutputDomain(_externalDomain);
  _n = NoiseFactory::create(_noise);
}

Domain DataGenericProximitySensor::getInternalDomain(int index)
{
  YM_LOCK;
  Domain d = _internalDomain;
  YM_UNLOCK;
  return d;
}

Domain DataGenericProximitySensor::getExternalDomain(int index)
{
  YM_LOCK;
  Domain d = _externalDomain;
  YM_UNLOCK;
  return d;
}

void DataGenericProximitySensor::_resetTo(const DataSensor *sensor)
{
  DataGenericProximitySensor *other = (DataGenericProximitySensor*)sensor;
  _name          = other->name();
  _object        = other->object();
  _pose          = other->pose();
  _distance      = other->distance();
  _mapping       = other->mapping();
  _openingAngles = other->openingAngles();
  _noise         = other->noise();
  _filter        = other->filter();
  _measuredDistance = other->getMeasuredDistance();
  __setMapping();
}
