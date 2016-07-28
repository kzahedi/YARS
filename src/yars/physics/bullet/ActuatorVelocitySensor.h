#ifndef __ACTUATOR_VELOCITY_SENSOR_H__
#define __ACTUATOR_VELOCITY_SENSOR_H__

#include <yars/physics/bullet/Sensor.h>

#include <yars/configuration/YarsConfiguration.h>
#include <yars/configuration/data/DataGenericVelocitySensor.h>

#include <vector>

class ActuatorVelocitySensor : public Sensor
{
  public:
    ActuatorVelocitySensor(DataGenericVelocitySensor *data, Robot *robot);
    ~ActuatorVelocitySensor();

    void prePhysicsUpdate();
    void postPhysicsUpdate();

  private:
    DataGenericVelocitySensor *_data;
    int _type;
    DataHingeActuator  *_hinge;
    DataSliderActuator *_slider;
};

#endif // __ACTUATOR_VELOCITY_SENSOR_H__


