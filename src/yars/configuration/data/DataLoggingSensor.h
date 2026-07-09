#ifndef __DATA_LOGGING_SENSOR_H__
#define __DATA_LOGGING_SENSOR_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataSensors.h>


#include <vector>
#include <string>

#define YARS_STRING_LOGGING_SENSOR            (char*)"sensor"
#define YARS_STRING_LOGGING_SENSOR_DEFINITION (char*)"sensor_logging_definition"

#define INTERNAL 6602
#define EXTERNAL 6603

class DataLoggingSensor : public DataNode, public std::vector<string>
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataLoggingSensor(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataLoggingSensor();

    /**
     * @brief Adds a DataParseElement to this node.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataLoggingSensor* copy();


    string target();
    void setTarget(string target);

    DataSensor* sensor();
    void set(DataSensor* sensor);
    int precision();
    void setPrecision(int precision);
    std::vector<int> order();

  private:
    string      _target;
    DataSensor* _sensor;
    int         _precision;
    std::vector<int> _order;

};

#endif // __DATA_LOGGING_SENSOR_H__


