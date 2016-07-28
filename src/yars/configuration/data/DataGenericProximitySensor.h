#ifndef __DATA_GENERIC_PROXIMITY_SENSOR_H__
#define __DATA_GENERIC_PROXIMITY_SENSOR_H__

#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>

#include <yars/types/Pose.h>
#include <yars/types/P2D.h>
#include <yars/types/Domain.h>

#include <yars/util/Mapping.h>
#include <yars/util/noise/Noise.h>

#include <pthread.h>

# define GENERIC_PROXIMITY                            "proximity"
# define YARS_STRING_GENERIC_PROXIMITY                 (char*)GENERIC_PROXIMITY
# define YARS_STRING_GENERIC_PROXIMITY_DEFINITION      (char*)GENERIC_PROXIMITY DIVIDER DEFINITION

class DataGenericProximitySensor : public DataSensor
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataGenericProximitySensor(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataGenericProximitySensor();

    /**
     * @brief Returns the pose with respect to the origin of the attaching object.
     *
     * @return
     */
    Pose pose();

    /**
     * @brief Returns the minimal and maximal measured distance that this
     * sensors will sense.
     *
     * @return domain
     * @sa Domain
     */
    yReal distance();

    /**
     * @brief Returns the opening angles in x and y direction.
     *
     * @return opening angles
     * @sa P2D
     */
    P2D openingAngles();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    static void createXsd(XsdSpecification *spec);

    DataGenericProximitySensor *_copy();

    void setMeasuredDistance(yReal length);
    yReal getMeasuredDistance();

    int dimension() { return 1; }

    yReal internalValue(int index);
    yReal externalValue(int index);
    void setInternalValue(int index, yReal v);
    void setExternalValue(int index, yReal v);
    Domain getInternalDomain(int index);
    Domain getExternalDomain(int index);

    void _resetTo(const DataSensor *sensor);

  private:
    void       __setMapping();
    Pose        _pose;
    yReal    _distance;
    P2D         _openingAngles;
    yReal    _measuredDistance;

    yReal _internalValue;
    yReal _externalValue;
    Mapping  _internalExternalMapping;
    Domain   _internalDomain;
    Domain   _externalDomain;
    Noise   *_n;

    pthread_mutex_t _mutex;

};

#endif // __DATA_GENERIC_PROXIMITY_SENSOR_H__
