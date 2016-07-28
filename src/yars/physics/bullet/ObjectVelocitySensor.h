#ifndef __OBJECT_VELOCITY_SENSOR_H__
#define __OBJECT_VELOCITY_SENSOR_H__

#include <yars/physics/bullet/Sensor.h>

#include <yars/configuration/YarsConfiguration.h>
#include <yars/configuration/data/DataObjectVelocitySensor.h>

#include <vector>

class ObjectVelocitySensor : public Sensor
{
  public:
    ObjectVelocitySensor(DataObjectVelocitySensor *data, Robot *robot);
    ~ObjectVelocitySensor();

    void prePhysicsUpdate();
    void postPhysicsUpdate();

  private:
    DataObjectVelocitySensor *_data;
};
#endif // __OBJECT_VELOCITY_SENSOR_H__



