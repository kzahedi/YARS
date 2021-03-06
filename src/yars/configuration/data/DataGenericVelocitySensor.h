#ifndef __DATA_GENERIC_VELOCITY_SENSOR_H__
#define __DATA_GENERIC_VELOCITY_SENSOR_H__

#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>

#include <yars/types/Domain.h>

#include <yars/util/Mapping.h>
#include <yars/util/noise/Noise.h>

# define YARS_STRING_GENERIC_VELOCITY_SENSOR            (char*)"velocity"
# define YARS_STRING_GENERIC_VELOCITY_SENSOR_DEFINITION (char*)"velocity_definition"

#include <string>
#include <pthread.h>

using namespace std;

class DataGenericVelocitySensor : public DataSensor
{
  public:

    /**
     * @brief Constructor
     *
     * @param parent
     */
    DataGenericVelocitySensor(DataNode* parent);

    /**
     * @brief Destructor
     *
     * @param parent
     */
    ~DataGenericVelocitySensor();

    /**
     * @brief Returns the domain of the sensor values. These values define the
     * input/output interface of the sensor.
     *
     * @return domain
     * @sa Domain
     */
    Domain domain();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    static void createXsd(XsdSpecification *spec);

    DataGenericVelocitySensor *_copy();

    int dimension() { return 1; }

    double internalValue(int index);
    double externalValue(int index);
    void setInternalValue(int index, double v);
    void setExternalValue(int index, double v);
    Domain getInternalDomain(int index);
    Domain getExternalDomain(int index);

    void _resetTo(const DataSensor *sensor);

  private:
    void __setMapping();
    Domain      _domain;
    double _internalValue;
    double _externalValue;
    Mapping  _internalExternalMapping;
    Domain   _internalDomain;
    Domain   _externalDomain;
    Noise   *_n;
    pthread_mutex_t _mutex;
};

#endif // __DATA_GENERIC_VELOCITY_SENSOR_H__
