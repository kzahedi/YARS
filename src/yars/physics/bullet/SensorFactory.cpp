#include "SensorFactory.h"
#include "PositionSensor.h"
#include "OrientationSensor.h"
#include "BinaryContactSensor.h"
#include "ActuatorDeflectionSensor.h"
#include "ActuatorVelocitySensor.h"
#include "GenericProximitySensor.h"
#include "GenericActuatorSensor.h"
#include "GenericLDRSensor.h"
#include "ObjectVelocitySensor.h"
#include "ObjectAngularVelocitySensor.h"
#include "MuscleSensor.h"

Sensor* SensorFactory::create(DataSensor *sensor, Robot *robot)
{
  switch(sensor->type())
  {
    case DATA_GENERIC_POSITION_SENSOR:
      return __createPosition(sensor, robot);
      break;
    case DATA_GENERIC_ORIENTATION_SENSOR:
      return __createOrientation(sensor, robot);
      break;
    case DATA_GENERIC_BINARY_CONTACT_SENSOR:
      return __createBinaryContact(sensor, robot);
      break;
    case DATA_GENERIC_DEFLECTION_SENSOR:
      return __createActuatorDeflection(sensor, robot);
      break;
    case DATA_GENERIC_VELOCITY_SENSOR:
      return __createActuatorVelocity(sensor, robot);
      break;
    case DATA_GENERIC_PROXIMITY_SENSOR:
      return __createGenericProximity(sensor, robot);
      break;
    case DATA_GENERIC_ACTUATOR_SENSOR:
      return __createGenericActuatorSensor(sensor, robot);
      break;
    case DATA_GENERIC_OBJECT_VELOCITY_SENSOR:
      return __createObjectVelocitySensor(sensor, robot);
      break;
    case DATA_GENERIC_OBJECT_ANGULAR_VELOCITY_SENSOR:
      return __createObjectAngularVelocitySensor(sensor, robot);
      break;
    case DATA_GENERIC_LIGHT_DEPENDENT_RESISTOR_SENSOR:
      return __createLDRSensor(sensor, robot);
      break;
    case DATA_MUSCLE_SENSOR:
      return __createMuscleSensor(sensor, robot);
      break;  }
  YarsErrorHandler::push("SensorFactory::create unknown sensor type given %d", sensor->type());
  return NULL;
}

Sensor* SensorFactory::__createMuscleSensor(DataSensor *sensor, Robot *robot)
{
  DataMuscleSensor *data = (DataMuscleSensor*)sensor;
  MuscleSensor *s = new MuscleSensor(data, robot);
  return s;
}

Sensor* SensorFactory::__createLDRSensor(DataSensor *sensor, Robot *robot)
{
  DataGenericLightDependentResistorSensor *data = (DataGenericLightDependentResistorSensor*)sensor;
  GenericLDRSensor *s = new GenericLDRSensor(data, robot);
  return s;
}

Sensor* SensorFactory::__createPosition(DataSensor *sensor, Robot *robot)
{
  DataGenericPositionSensor *data = (DataGenericPositionSensor*)sensor;
  PositionSensor *s = new PositionSensor(data, robot);
  return s;
}

Sensor* SensorFactory::__createOrientation(DataSensor *sensor, Robot *robot)
{
  DataGenericOrientationSensor *data = (DataGenericOrientationSensor*)sensor;
  OrientationSensor *s = new OrientationSensor(data, robot);
  return s;
}

Sensor* SensorFactory::__createBinaryContact(DataSensor *sensor, Robot *robot)
{
  DataGenericBinaryContactSensor *data = (DataGenericBinaryContactSensor*)sensor;
  BinaryContactSensor *s = new BinaryContactSensor(data, robot);
  return s;
}

Sensor* SensorFactory::__createActuatorDeflection(DataSensor *sensor, Robot *robot)
{
  DataGenericDeflectionSensor *data = (DataGenericDeflectionSensor*)sensor;
  ActuatorDeflectionSensor *s = new ActuatorDeflectionSensor(data, robot);
  return s;
}

Sensor* SensorFactory::__createActuatorVelocity(DataSensor *sensor, Robot *robot)
{
  DataGenericVelocitySensor *data = (DataGenericVelocitySensor*)sensor;
  ActuatorVelocitySensor *s = new ActuatorVelocitySensor(data, robot);
  return s;
}

Sensor* SensorFactory::__createGenericProximity(DataSensor *sensor, Robot *robot)
{
  DataGenericProximitySensor *data = (DataGenericProximitySensor*)sensor;
  GenericProximitySensor *s = new GenericProximitySensor(data, robot);
  return s;
}

Sensor* SensorFactory::__createGenericActuatorSensor(DataSensor *sensor, Robot *robot)
{
  DataGenericActuatorSensor *data = (DataGenericActuatorSensor*)sensor;
  GenericActuatorSensor *s = new GenericActuatorSensor(data, robot);
  return s;
}

Sensor* SensorFactory::__createObjectVelocitySensor(DataSensor *sensor, Robot *robot)
{
  DataObjectVelocitySensor *data = (DataObjectVelocitySensor*)sensor;
  ObjectVelocitySensor *s = new ObjectVelocitySensor(data, robot);
  return s;
}

Sensor* SensorFactory::__createObjectAngularVelocitySensor(DataSensor *sensor, Robot *robot)
{
  DataObjectAngularVelocitySensor *data = (DataObjectAngularVelocitySensor*)sensor;
  ObjectAngularVelocitySensor *s        = new ObjectAngularVelocitySensor(data, robot);
  return s;
}
