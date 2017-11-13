#ifndef __DATA_SENSOR_H__
#define __DATA_SENSOR_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>

#include <yars/types/Pose.h>
#include <yars/types/Domain.h>

#include <vector>

#define DATA_GENERIC_VELOCITY_SENSOR                 2500
#define DATA_GENERIC_DEFLECTION_SENSOR               2501
#define DATA_GENERIC_POSITION_SENSOR                 2502
#define DATA_GENERIC_ORIENTATION_SENSOR              2503
#define DATA_GENERIC_ACTUATOR_SENSOR                 2504
#define DATA_GENERIC_OBJECT_VELOCITY_SENSOR          2505
#define DATA_GENERIC_OBJECT_ANGULAR_VELOCITY_SENSOR  2506
#define DATA_GENERIC_BINARY_CONTACT_SENSOR           2507
#define DATA_GENERIC_PROXIMITY_SENSOR                2508
#define DATA_GENERIC_LIGHT_DEPENDENT_RESISTOR_SENSOR 2509

// TODO: not yet implemented
#define DATA_GENERIC_CAMERA_SENSOR                   3507
#define DATA_GENERIC_AMBIENT_LIGHT_SENSOR            3508
#define DATA_GENERIC_INTERNAL_ENERGY_SENSOR          3509
#define DATA_SHARP_DM2Y3A003K0F_SENSOR               3511
#define DATA_SHARP_GP2D12_37_SENSOR                  3512
#define DATA_GENERIC_FEEDBACK_SENSOR                 3513
#define DATA_GENERIC_SIGNAL_SENSOR                   3514

using namespace std;


/**
 * @brief Base class for all sensor implementations.
 */
class DataSensor : public DataNode
{
  public:
    DataSensor(DataNode *parent, int type);
    virtual ~DataSensor();

    int type();

    void applyOffset(Pose);

    DataSensor* copy();
    void resetTo(const DataSensor* other);

    /**
     * @brief Returns the given name of the sensor.
     *
     * @return
     */
    string name();

    /**
     * @brief Returns the noise definition.
     *
     * @return noise definition
     * @sa DataNoise
     */
    DataNoise* noise();

    /**
     * @brief Returns the filter definition.
     *
     * @return filter definition.
     * @sa DataFilter
     */
    DataFilter* filter();

    /**
     * @brief Returns the mapping of the sensor values. These values define the
     * input/output interface of the sensor.
     *
     * @return domain
     * @sa Domain
     */
    Domain mapping();

    /**
     * @brief Returns the object to which this sensor is attached.
     *
     * @return name of the object.
     */
    string object();

    virtual DataSensor* _copy()                          = 0;
    virtual void _resetTo(const DataSensor* other)       = 0;
    virtual int dimension()                              = 0;

    virtual double internalValue(int index)            = 0;
    virtual double externalValue(int index)            = 0;
    virtual void setInternalValue(int index, double v) = 0;
    virtual void setExternalValue(int index, double v) = 0;

    virtual Domain getInternalDomain(int index)          = 0;
    virtual Domain getExternalDomain(int index)          = 0;

    virtual string logName(int index);

  protected:
    int         _type;
    string      _name;
    DataNoise  *_noise;
    DataFilter *_filter;
    Domain      _mapping;
    string      _object;
};

#endif // __DATA_SENSOR_H__
