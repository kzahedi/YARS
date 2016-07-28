#ifndef __DATA_GENERIC_INTERNAL_ENERGY_SENSOR_H__
#define __DATA_GENERIC_INTERNAL_ENERGY_SENSOR_H__

#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>

#include <yars/types/Domain.h>

# define GENERIC_INTERNAL_ENERGY_SENSOR "internalEnergy"
# define YARS_STRING_GENERIC_INTERNAL_ENERGY_SENSOR  (char*)GENERIC_INTERNAL_ENERGY_SENSOR
# define YARS_STRING_GENERIC_INTERNAL_ENERGY_SENSOR_DEFINITION  (char*)GENERIC_INTERNAL_ENERGY_SENSOR DIVIDER DEFINITION

#include <string>

using namespace std;

class DataGenericInternalEnergySensor : public DataSensor
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataGenericInternalEnergySensor(DataNode* parent);

    /**
     * @brief Destructor.
     */
    ~DataGenericInternalEnergySensor();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    static void createXsd(XsdSpecification *spec);

    DataGenericInternalEnergySensor *_copy();

    int dimension() { return 1; }

    yReal internalValue(int index) {return 0;};
    yReal externalValue(int index) {return 0;};
    void setInternalValue(int index, yReal v) { };
    void setExternalValue(int index, yReal v) { };
    Domain getInternalDomain(int index) { return Domain(); };
    Domain getExternalDomain(int index) { return Domain(); };

    void _resetTo(const DataSensor *sensor);

  private:

};

#endif // __DATA_GENERIC_INTERNAL_ENERGY_SENSOR_H__
