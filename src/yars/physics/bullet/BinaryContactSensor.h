#ifndef __BINARY_CONTACT_SENSOR_H__
#define __BINARY_CONTACT_SENSOR_H__

#include <yars/physics/bullet/Sensor.h>

#include <yars/configuration/YarsConfiguration.h>
#include <yars/configuration/data/DataGenericBinaryContactSensor.h>

#include <vector>

class BinaryContactSensor : public Sensor
{
  public:
    BinaryContactSensor(DataGenericBinaryContactSensor *data, Robot *robot);
    ~BinaryContactSensor();

    void prePhysicsUpdate();
    void postPhysicsUpdate();

  private:
    DataGenericBinaryContactSensor *_data;
};
#endif // __BINARY_CONTACT_SENSOR_H__



