#ifndef __GENERIC_ACTUATOR_SENSOR_H__
#define __GENERIC_ACTUATOR_SENSOR_H__

#include <yars/physics/bullet/Sensor.h>

#include <yars/configuration/YarsConfiguration.h>
#include <yars/configuration/data/DataGenericActuatorSensor.h>
#include <yars/configuration/data/DataGenericActuator.h>

#include <vector>

class GenericActuatorSensor : public Sensor
{
  public:
    GenericActuatorSensor(DataGenericActuatorSensor *data, Robot *robot);
    ~GenericActuatorSensor();

    void prePhysicsUpdate();
    void postPhysicsUpdate();

  private:
    DataGenericActuatorSensor *_data;
    DataGenericActuator       *_aData;
};

#endif // __GENERIC_ACTUATOR_SENSOR_H__

