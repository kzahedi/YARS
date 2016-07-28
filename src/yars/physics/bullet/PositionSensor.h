#ifndef __POSITION_SENSOR_H__
#define __POSITION_SENSOR_H__

#include <yars/physics/bullet/Sensor.h>

#include <yars/configuration/YarsConfiguration.h>
#include <yars/configuration/data/DataGenericPositionSensor.h>

#include <vector>

class PositionSensor : public Sensor
{
  public:
    PositionSensor(DataGenericPositionSensor *data, Robot *robot);
    ~PositionSensor();

    void prePhysicsUpdate();
    void postPhysicsUpdate();

  private:
    DataGenericPositionSensor *_data;
};
#endif // __POSITION_SENSOR_H__



