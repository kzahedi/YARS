#ifndef __DATA_SENSOR_FACTORY_H__
#define __DATA_SENSOR_FACTORY_H__

#include "DataSensor.h"

# define YARS_STRING_SENSOR_LIST            (char*)"sensors"
# define YARS_STRING_SENSOR_LIST_DEFINITION (char*)"sensors_definition"

class DataSensorFactory
{
  public:
    /**
     * @brief Returns a sensors, if element is the opening tag of one, or NULL
     * otherwise
     *
     * @param element
     * @param parent
     *
     * @return NULL or DataSensor*
     *
     * @sa DataGenericProximitySensor
     * @sa DataGenericCameraSensor
     * @sa DataGenericAmbientLightSensor
     * @sa DataGenericInternalEnergySensor
     * @sa DataGenericOrientationSensor
     * @sa DataGenericPositionSensor
     * @sa DataGenericLightDependentResistorSensor
     * @sa DataSharpDM2Y3A003K0FSensor
     * @sa DataSharpGP2D12_37Sensor
     * @sa DataGenericFeedbackSensor
     * @sa DataGenericVelocitySensor
     * @sa DataGenericDeflectionSensor
     * @sa DataGenericSignalSensor
     * @sa DataGenericBinaryContactSensor
     *
     */
    static DataSensor* sensor(DataParseElement *element, DataNode *parent);

    static void createXsd(XsdSpecification *spec);

  private:
    static DataSensor* __generateGenericProximitySensor      (DataParseElement *element, DataNode *parent);
    static DataSensor* __generateGenericCameraSensor         (DataParseElement *element, DataNode *parent);
    static DataSensor* __generateGenericAmbientLightSensor   (DataParseElement *element, DataNode *parent);
    static DataSensor* __generateGenericInternalEnergySensor (DataParseElement *element, DataNode *parent);
    static DataSensor* __generateGenericOrientationSensor    (DataParseElement *element, DataNode *parent);
    static DataSensor* __generateGenericPositionSensor       (DataParseElement *element, DataNode *parent);
    static DataSensor* __generateGenericLDRSensor            (DataParseElement *element, DataNode *parent);
    static DataSensor* __generateSharpDM2Y3A003K0FSensor     (DataParseElement *element, DataNode *parent);
    static DataSensor* __generateSharpGP2D12_37Sensor        (DataParseElement *element, DataNode *parent);
    static DataSensor* __generateGenericFeedbackSensor       (DataParseElement *element, DataNode *parent);
    static DataSensor* __generateGenericVelocitySensor       (DataParseElement *element, DataNode *parent);
    static DataSensor* __generateGenericDeflectionSensor     (DataParseElement *element, DataNode *parent);
    static DataSensor* __generateGenericSignalSensor         (DataParseElement *element, DataNode *parent);
    static DataSensor* __generateGenericBinaryContactSensor  (DataParseElement *element, DataNode *parent);
    static DataSensor* __generateGenericActuatorSensor       (DataParseElement *element, DataNode *parent);
    static DataSensor* __generateObjectVelocitySensor        (DataParseElement *element, DataNode *parent);
    static DataSensor* __generateObjectAngularVelocitySensor (DataParseElement *element, DataNode *parent);
};

#endif // __DATA_SENSOR_FACTORY_H__
