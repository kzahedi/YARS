#ifndef __ORIENTATION_SENSOR_H__
#define __ORIENTATION_SENSOR_H__

#include <yars/physics/bullet/Sensor.h>

#include <yars/configuration/YarsConfiguration.h>
#include <yars/configuration/data/DataGenericOrientationSensor.h>

#include <vector>

class OrientationSensor : public Sensor
{
  public:
    OrientationSensor(DataGenericOrientationSensor *data, Robot *robot);
    ~OrientationSensor();

    void prePhysicsUpdate();
    void postPhysicsUpdate();

  private:
    DataGenericOrientationSensor *_data;
};
#endif // __ORIENTATION_SENSOR_H__




