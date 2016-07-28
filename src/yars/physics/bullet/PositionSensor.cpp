#include "PositionSensor.h"

PositionSensor::PositionSensor(DataGenericPositionSensor *data, Robot *robot)
  : Sensor(data->name(), data->object(), robot)
{
  _data = data;
}

PositionSensor::~PositionSensor()
{
}

void PositionSensor::prePhysicsUpdate()
{
}

void PositionSensor::postPhysicsUpdate()
{
  int index = 0;
  if(_data->x()) _data->setInternalValue(index++,  _targetObject->data()->pose().position.x);
  if(_data->y()) _data->setInternalValue(index++,  _targetObject->data()->pose().position.y);
  if(_data->z()) _data->setInternalValue(index++,  _targetObject->data()->pose().position.z);
}

