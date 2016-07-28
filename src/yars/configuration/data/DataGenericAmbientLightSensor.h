#ifndef __DATA_GENERIC_AMBIENT_LIGHT_SENSOR_H__
#define __DATA_GENERIC_AMBIENT_LIGHT_SENSOR_H__

#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>

#include <yars/types/Domain.h>
#include <yars/util/Mapping.h>
#include <yars/util/noise/Noise.h>

#include <pthread.h>


# define GENERIC_AMBIENT_LIGHT_SENSOR                 "ambientLight"
# define YARS_STRING_GENERIC_AMBIENT_LIGHT_SENSOR  (char*)GENERIC_AMBIENT_LIGHT_SENSOR
# define YARS_STRING_GENERIC_AMBIENT_LIGHT_SENSOR_DEFINITION  (char*)GENERIC_AMBIENT_LIGHT_SENSOR DIVIDER DEFINITION

class DataGenericAmbientLightSensor : public DataSensor
{
  public:

    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataGenericAmbientLightSensor(DataNode* parent);

    /**
     * @brief Destructor.
     */
    ~DataGenericAmbientLightSensor();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataGenericAmbientLightSensor* _copy();

    static void createXsd(XsdSpecification *spec);

    int dimension() { return 1; }

    yReal internalValue(int index);
    yReal externalValue(int index);
    void setInternalValue(int index, yReal v);
    void setExternalValue(int index, yReal v);
    Domain getInternalDomain(int index);
    Domain getExternalDomain(int index);

    void _resetTo(const DataSensor *sensor);

  private:
    void __setMapping();

    yReal   _internalValue;
    yReal   _externalValue;
    Mapping _internalExternalMapping;
    Domain  _internalDomain;
    Domain  _externalDomain;
    pthread_mutex_t _mutex;
    Noise   *_n;

};

#endif // __DATA_GENERIC_AMBIENT_LIGHT_SENSOR_H__
