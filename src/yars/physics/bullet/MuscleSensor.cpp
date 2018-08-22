#include "MuscleSensor.h"


#include <math.h>

MuscleSensor::MuscleSensor(DataMuscleSensor *data, Robot *robot)
  : Sensor(data->name(), data->object(), robot)
{
  _data = data;
  _actuatorData = (DataMuscleActuator*)_targetActuator->data();
}

MuscleSensor::~MuscleSensor()
{
}

void MuscleSensor::prePhysicsUpdate()
{
}

void MuscleSensor::postPhysicsUpdate()
{
  _data->setInternalValue(0, _actuatorData->getAppliedForce(0));
  _data->setInternalValue(1, _actuatorData->getVelocityComponent());
  _data->setInternalValue(2, _actuatorData->getLengthComponent());
  _data->setInternalValue(3, _actuatorData->getMuscleVelocity());
  _data->setInternalValue(4, _actuatorData->getMuscleLength());
  _data->setInternalValue(5, _actuatorData->getAppliedVelocity(0));
}