#ifndef __DATA_GENERIC_SIGNAL_SENSOR_H__
#define __DATA_GENERIC_SIGNAL_SENSOR_H__

#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>

#include <yars/types/Domain.h>

# define GENERIC_SIGNAL_SENSOR "signal"
# define YARS_STRING_GENERIC_SIGNAL_SENSOR  (char*)GENERIC_SIGNAL_SENSOR
# define YARS_STRING_GENERIC_SIGNAL_SENSOR_DEFINITION  (char*)GENERIC_SIGNAL_SENSOR DIVIDER DEFINITION

#include <string>

using namespace std;

class DataGenericSignalSensor : public DataSensor
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataGenericSignalSensor(DataNode* parent);

    /**
     * @brief Destructor.
     */
    ~DataGenericSignalSensor();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    /**
     * @brief Returns the signal to which this sensor listens to.
     *
     * @return
     */
    string signal();

    static void createXsd(XsdSpecification *spec);

    DataGenericSignalSensor* _copy();

    int dimension() { return 0; } // not implemented yet

    double internalValue(int index) {return 0;};
    double externalValue(int index) {return 0;};
    void setInternalValue(int index, double v) { };
    void setExternalValue(int index, double v) { };
    Domain getInternalDomain(int index) { return Domain(0,1); };
    Domain getExternalDomain(int index) { return Domain(0,1); };

    void _resetTo(const DataSensor *sensor);

  private:
    string      _signal;
};

#endif // __DATA_GENERIC_SIGNAL_SENSOR_H__
