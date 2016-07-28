#include "OrientationSensor.h"

#include <math.h>

OrientationSensor::OrientationSensor(DataGenericOrientationSensor *data, Robot *robot)
  : Sensor(data->name(), data->object(), robot)
{
  _data = data;
}

OrientationSensor::~OrientationSensor()
{
}

void OrientationSensor::prePhysicsUpdate()
{
}

void OrientationSensor::postPhysicsUpdate()
{
  int index = 0;

  if(_data->useDegree())
  {
    if(_data->x()) _data->setInternalValue(index++, RAD_TO_DEG(_targetObject->data()->pose().orientation.x));
    if(_data->y()) _data->setInternalValue(index++, RAD_TO_DEG(_targetObject->data()->pose().orientation.y));
    if(_data->z()) _data->setInternalValue(index++, RAD_TO_DEG(_targetObject->data()->pose().orientation.z));
  }
  else
  {
    if(_data->x()) _data->setInternalValue(index++, _targetObject->data()->pose().orientation.x);
    if(_data->y()) _data->setInternalValue(index++, _targetObject->data()->pose().orientation.y);
    if(_data->z()) _data->setInternalValue(index++, _targetObject->data()->pose().orientation.z);
  }
}


