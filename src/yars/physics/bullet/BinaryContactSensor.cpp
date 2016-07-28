#include "BinaryContactSensor.h"
#include "CollisionCallback.h"

BinaryContactSensor::BinaryContactSensor(DataGenericBinaryContactSensor *data, Robot *robot)
  : Sensor(data->name(), data->object(), robot)
{
  _data    = data;
  CollisionCallback::instance()->addObject(_targetObject);
}

BinaryContactSensor::~BinaryContactSensor()
{
}

void BinaryContactSensor::prePhysicsUpdate()
{
  _targetObject->unsetCollided();
}

void BinaryContactSensor::postPhysicsUpdate()
{
  _data->setInternalValue(0, (_targetObject->isCollided())?1:0);
}

