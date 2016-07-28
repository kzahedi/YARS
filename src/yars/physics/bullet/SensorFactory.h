#ifndef __SENSOR_FACTORY_H__
#define __SENSOR_FACTORY_H__

class Robot;

#include <yars/physics/bullet/Sensor.h>

#include <yars/configuration/YarsConfiguration.h>

class SensorFactory
{
  public:
    static Sensor* create(DataSensor *sensor, Robot *robot);

  private:
    static Sensor* __createPosition                    (DataSensor *sensor, Robot *robot);
    static Sensor* __createOrientation                 (DataSensor *sensor, Robot *robot);
    static Sensor* __createBinaryContact               (DataSensor *sensor, Robot *robot);
    static Sensor* __createActuatorDeflection          (DataSensor *sensor, Robot *robot);
    static Sensor* __createActuatorVelocity            (DataSensor *sensor, Robot *robot);
    static Sensor* __createGenericProximity            (DataSensor *sensor, Robot *robot);
    static Sensor* __createGenericActuatorSensor       (DataSensor *sensor, Robot *robot);
    static Sensor* __createObjectVelocitySensor        (DataSensor *sensor, Robot *robot);
    static Sensor* __createObjectAngularVelocitySensor (DataSensor *sensor, Robot *robot);
    static Sensor* __createLDRSensor                   (DataSensor *sensor, Robot *robot);

};

#endif // __SENSOR_FACTORY_H__




