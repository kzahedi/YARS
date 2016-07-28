#ifndef __DATA_SENSORS_H__
#define __DATA_SENSORS_H__

class DataSensors : public std::vector<DataSensor*>
{
  public:
    DataSensors() { };

    void add(DataSensor* sensor)
    {
      push_back(sensor);
    }
};

#endif // __DATA_SENSORS_H__
