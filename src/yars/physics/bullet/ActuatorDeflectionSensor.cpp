#include <yars/physics/bullet/ActuatorDeflectionSensor.h>

#include <yars/util/macros.h>

ActuatorDeflectionSensor::ActuatorDeflectionSensor(DataGenericDeflectionSensor *data, Robot *robot)
  : Sensor(data->name(), data->object(), robot)
{
  _data = data;
  if(_targetActuator->data()->type() == DATA_ACTUATOR_HINGE) _data->setDegToRad();
}

ActuatorDeflectionSensor::~ActuatorDeflectionSensor()
{
}

void ActuatorDeflectionSensor::prePhysicsUpdate()
{
}

void ActuatorDeflectionSensor::postPhysicsUpdate()
{
  for(int i = 0; i < _data->dimension(); i++)
  {
    _data->setInternalValue(i, _targetActuator->data()->internalValue(i));
  }
}
