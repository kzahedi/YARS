#ifndef __DATA_GENERIC_DEFLECTION_SENSOR_H__
#define __DATA_GENERIC_DEFLECTION_SENSOR_H__

#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>

#include <yars/types/Domain.h>

#include <yars/util/Mapping.h>
#include <yars/util/noise/Noise.h>

# define YARS_STRING_GENERIC_DEFLECTION_SENSOR            (char*)"deflection"
# define YARS_STRING_GENERIC_DEFLECTION_SENSOR_DEFINITION (char*)"deflection_definition"

#include <string>
#include <pthread.h>

using namespace std;

class DataGenericDeflectionSensor : public DataSensor
{
  public:

    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataGenericDeflectionSensor(DataNode* parent);

    /**
     * @brief Destructor
     */
    ~DataGenericDeflectionSensor();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    /**
     * @brief Returns the domain of the sensor, i.e. which light values can be
     * measured.
     *
     * @return domain
     */
    Domain domain();

    static void createXsd(XsdSpecification *spec);

    DataGenericDeflectionSensor* _copy();

    int dimension() { return 1; }

    double internalValue(int index);
    double externalValue(int index);
    void setInternalValue(int index, double v);
    void setExternalValue(int index, double v);
    void setInternalDomain(Domain d);
    void setExternalDomain(Domain d);
    Domain getInternalDomain(int index);
    Domain getExternalDomain(int index);

    void _resetTo(const DataSensor *sensor);
    void setDegToRad();

  private:
    void __setMapping();
    Domain   _domain;
    double _internalValue;
    double _externalValue;
    Mapping  _internalExternalMapping;
    Domain   _internalDomain;
    Domain   _externalDomain;
    Noise   *_n;
    pthread_mutex_t _mutex;
    bool  _useRad;
};

#endif // __DATA_GENERIC_DEFLECTION_SENSOR_H__
