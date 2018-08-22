#ifndef __MUSCLE_SENSOR_H__
#define __MUSCLE_SENSOR_H__

#include <yars/physics/bullet/Sensor.h>

#include <yars/configuration/YarsConfiguration.h>
#include <yars/configuration/data/DataMuscleSensor.h>

#include <vector>

class MuscleSensor : public Sensor
{
  public:
    MuscleSensor(DataMuscleSensor *data, Robot *robot);
    ~MuscleSensor();

    void prePhysicsUpdate();
    void postPhysicsUpdate();

  private:
    DataMuscleSensor *_data;
    DataMuscleActuator *_actuatorData;
};
#endif // __MUSCLE_SENSOR_H__




