#include <yars/physics/bullet/GenericActuatorSensor.h>

#include <yars/util/macros.h>

#include <iostream>


GenericActuatorSensor::GenericActuatorSensor(DataGenericActuatorSensor *data, Robot *robot)
  : Sensor(data->name(), data->object(), robot)
{
  _data = data;
  _aData = (DataGenericActuator*)_targetActuator->data();
}

GenericActuatorSensor::~GenericActuatorSensor()
{
}

void GenericActuatorSensor::prePhysicsUpdate()
{
}

void GenericActuatorSensor::postPhysicsUpdate()
{
  for(int i = 0; i < _data->dimension(); i++)
  {
    if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_ANGULAR)
    {
      if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_VELOCITY)
      {
        if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_X)
          _data->setInternalValue(i, _aData->getAngularVelocity(0));
        if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_Y)
          _data->setInternalValue(i, _aData->getAngularVelocity(1));
        if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_Z)
          _data->setInternalValue(i, _aData->getAngularVelocity(2));
      }
      else if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_DEFLECTION)
      {
        if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_X) _data->setInternalValue(i, _aData->getAngle(0));
        if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_Y) _data->setInternalValue(i, _aData->getAngle(1));
        if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_Z) _data->setInternalValue(i, _aData->getAngle(2));
      }
      else
      {
        YarsErrorHandler::push("GenericActuatorSensor::postPhysicsUpdate. Angular sensor is not VELOCITY or DEFLECTION");
      }
    }
    else if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_LINEAR)
    {
      if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_VELOCITY)
      {
        if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_X)
          _data->setInternalValue(i, _aData->getLinearVelocity(0));
        if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_Y)
          _data->setInternalValue(i, _aData->getLinearVelocity(1));
        if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_Z)
          _data->setInternalValue(i, _aData->getLinearVelocity(2));
      }
      else if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_DEFLECTION)
      {
        std::cout << _aData->getPosition(0) << std::endl;

        if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_X)
          _data->setInternalValue(i, _aData->getPosition(0));
        if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_Y)
          _data->setInternalValue(i, _aData->getPosition(1));
        if(_data->type(i) & GENERIC_ACTUATOR_SENSOR_Z)
          _data->setInternalValue(i, _aData->getPosition(2));
      }
      else
      {
        YarsErrorHandler::push("GenericActuatorSensor::postPhysicsUpdate. Linear sensor is not VELOCITY or DEFLECTION");
      }
    }
    else
    {
      YarsErrorHandler::push("GenericActuatorSensor::postPhysicsUpdate. ERROR");
    }
  }
}
