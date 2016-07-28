#ifndef __GENERIC_LDR_SENSOR_H__
#define __GENERIC_LDR_SENSOR_H__

#include <yars/physics/bullet/Sensor.h>

#include <yars/configuration/YarsConfiguration.h>
#include <yars/configuration/data/DataGenericLightDependentResistorSensor.h>

#include <vector>

class GenericLDRSensor : public Sensor
{
  public:
    GenericLDRSensor(DataGenericLightDependentResistorSensor *data, Robot *robot);
    ~GenericLDRSensor();

    void prePhysicsUpdate();
    void postPhysicsUpdate();

  private:
    DataGenericLightDependentResistorSensor* _data;
    DataEnvironment*  _env;
    DataAmbientLight* _ambientLight;
    Pose              _pose;
    yReal             _z;
};
#endif // __GENERIC_LDR_SENSOR_H__



