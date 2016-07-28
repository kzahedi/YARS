#include "ActuatorVelocitySensor.h"

ActuatorVelocitySensor::ActuatorVelocitySensor(DataGenericVelocitySensor *data, Robot *robot)
  : Sensor(data->name(), data->object(), robot)
{
  _data   = data;
  _type   = _targetActuator->data()->type();
  _hinge  = NULL;
  _slider = NULL;
  switch(_type)
  {
    case DATA_ACTUATOR_HINGE:
      _hinge = (DataHingeActuator*)_targetActuator->data();
      break;
    case DATA_ACTUATOR_SLIDER:
      _slider = (DataSliderActuator*)_targetActuator->data();
      break;
  }
}

ActuatorVelocitySensor::~ActuatorVelocitySensor()
{ }

void ActuatorVelocitySensor::prePhysicsUpdate()
{ }

void ActuatorVelocitySensor::postPhysicsUpdate()
{
  switch(_type)
  {
    case DATA_ACTUATOR_HINGE:
        _data->setInternalValue(0, _hinge->getCurrentAngularVelocity());
        break;
    case DATA_ACTUATOR_SLIDER:
        _data->setInternalValue(0, _slider->getCurrentTransitionalVelocity());
        break;
  }
}

