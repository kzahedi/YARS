#ifndef __OBJECT_ANGULAR_VELOCITY_SENSOR_H__
#define __OBJECT_ANGULAR_VELOCITY_SENSOR_H__

#include <yars/physics/bullet/Sensor.h>

#include <yars/configuration/YarsConfiguration.h>
#include <yars/configuration/data/DataObjectAngularVelocitySensor.h>

#include <vector>

class ObjectAngularVelocitySensor : public Sensor
{
  public:
    ObjectAngularVelocitySensor(DataObjectAngularVelocitySensor *data, Robot *robot);
    ~ObjectAngularVelocitySensor();

    void prePhysicsUpdate();
    void postPhysicsUpdate();

  private:
    DataObjectAngularVelocitySensor *_data;
};
#endif // __OBJECT_ANGULAR_VELOCITY_SENSOR_H__



