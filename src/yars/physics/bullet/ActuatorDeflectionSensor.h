#ifndef __ACTUATOR_DEFLECTION_SENSOR_H__
#define __ACTUATOR_DEFLECTION_SENSOR_H__

#include <yars/physics/bullet/Sensor.h>

#include <yars/configuration/YarsConfiguration.h>
#include <yars/configuration/data/DataGenericDeflectionSensor.h>

#include <vector>

class ActuatorDeflectionSensor : public Sensor
{
  public:
    ActuatorDeflectionSensor(DataGenericDeflectionSensor *data, Robot *robot);
    ~ActuatorDeflectionSensor();

    void prePhysicsUpdate();
    void postPhysicsUpdate();

  private:
    DataGenericDeflectionSensor *_data;
};

#endif // __ACTUATOR_DEFLECTION_SENSOR_H__

