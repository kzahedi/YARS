#ifndef __DATA_GENERIC_BINARY_CONTACT_SENSOR_H__
#define __DATA_GENERIC_BINARY_CONTACT_SENSOR_H__

#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>

#include <yars/types/Domain.h>

#include <pthread.h>

# define YARS_STRING_GENERIC_BINARY_CONTACT_SENSOR            (char*)"binaryContact"
# define YARS_STRING_GENERIC_BINARY_CONTACT_SENSOR_DEFINITION (char*)"binaryContact_definition"

#include <string>

using namespace std;

class DataGenericBinaryContactSensor : public DataSensor
{
  public:
    /**
     * @brief Constructor
     *
     * @param parent
     */
    DataGenericBinaryContactSensor(DataNode* parent);

    /**
     * @brief Destructor
     */
    ~DataGenericBinaryContactSensor();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataGenericBinaryContactSensor* _copy();

    static void createXsd(XsdSpecification *spec);

    int dimension() { return 1; }

    double internalValue(int index);
    double externalValue(int index);
    void setInternalValue(int index, double v);
    void setExternalValue(int index, double v);
    Domain getInternalDomain(int index) { return Domain(0,1); };
    Domain getExternalDomain(int index) { return Domain(0,1); };

    void _resetTo(const DataSensor *sensor);

  private:

    double _value;
    pthread_mutex_t _mutex;
    Domain _domain;

};

#endif // __DATA_GENERIC_BINARY_CONTACT_SENSOR_H__
