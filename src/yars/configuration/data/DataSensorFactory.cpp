#include "DataSensorFactory.h"

#include "DataGenericProximitySensor.h"
#include "DataGenericCameraSensor.h"
#include "DataGenericAmbientLightSensor.h"
#include "DataGenericInternalEnergySensor.h"
#include "DataGenericOrientationSensor.h"
#include "DataGenericPositionSensor.h"
#include "DataGenericLightDependentResistorSensor.h"
#include "DataSharpDM2Y3A003K0FSensor.h"
#include "DataSharpGP2D12_37Sensor.h"
#include "DataGenericFeedbackSensor.h"
#include "DataGenericVelocitySensor.h"
#include "DataGenericDeflectionSensor.h"
#include "DataGenericSignalSensor.h"
#include "DataGenericBinaryContactSensor.h"
#include "DataGenericActuatorSensor.h"
#include "DataObjectVelocitySensor.h"
#include "DataObjectAngularVelocitySensor.h"
#include "DataMuscleSensor.h"


DataSensor* DataSensorFactory::sensor(DataParseElement *element, DataNode *parent)
{
  if(element->opening(YARS_STRING_GENERIC_PROXIMITY))                      return __generateGenericProximitySensor      (element, parent);
  if(element->opening(YARS_STRING_GENERIC_CAMERA_SENSOR))                  return __generateGenericCameraSensor         (element, parent);
  if(element->opening(YARS_STRING_GENERIC_AMBIENT_LIGHT_SENSOR))           return __generateGenericAmbientLightSensor   (element, parent);
  if(element->opening(YARS_STRING_GENERIC_ORIENTATION_SENSOR))             return __generateGenericOrientationSensor    (element, parent);
  if(element->opening(YARS_STRING_GENERIC_POSITION_SENSOR))                return __generateGenericPositionSensor       (element, parent);
  if(element->opening(YARS_STRING_GENERIC_INTERNAL_ENERGY_SENSOR))         return __generateGenericInternalEnergySensor (element, parent);
  if(element->opening(YARS_STRING_GENERIC_LDR_SENSOR))                     return __generateGenericLDRSensor            (element, parent);
  if(element->opening(YARS_STRING_SHARP_IR_DM2Y3A003K0F))                  return __generateSharpDM2Y3A003K0FSensor     (element, parent);
  if(element->opening(YARS_STRING_SHARP_IR_GP2D12_37))                     return __generateSharpGP2D12_37Sensor        (element, parent);
  if(element->opening(YARS_STRING_GENERIC_FEEDBACK_SENSOR))                return __generateGenericFeedbackSensor       (element, parent);
  if(element->opening(YARS_STRING_GENERIC_VELOCITY_SENSOR))                return __generateGenericVelocitySensor       (element, parent);
  if(element->opening(YARS_STRING_GENERIC_DEFLECTION_SENSOR))              return __generateGenericDeflectionSensor     (element, parent);
  if(element->opening(YARS_STRING_GENERIC_SIGNAL_SENSOR))                  return __generateGenericSignalSensor         (element, parent);
  if(element->opening(YARS_STRING_GENERIC_BINARY_CONTACT_SENSOR))          return __generateGenericBinaryContactSensor  (element, parent);
  if(element->opening(YARS_STRING_GENERIC_ACTUATOR_SENSOR))                return __generateGenericActuatorSensor       (element, parent);
  if(element->opening(YARS_STRING_GENERIC_OBJECT_VELOCITY_SENSOR))         return __generateObjectVelocitySensor        (element, parent);
  if(element->opening(YARS_STRING_GENERIC_OBJECT_ANGULAR_VELOCITY_SENSOR)) return __generateObjectAngularVelocitySensor (element, parent);
  if(element->opening(YARS_STRING_MUSCLE_SENSOR))                          return __generateMuscleSensor(element, parent);
  return NULL;
}

DataSensor* DataSensorFactory::__generateMuscleSensor(DataParseElement *element, DataNode *parent)
{
  DataMuscleSensor *sensor = new DataMuscleSensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateGenericProximitySensor(DataParseElement *element, DataNode *parent)
{
  DataGenericProximitySensor *sensor = new DataGenericProximitySensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateGenericCameraSensor(DataParseElement *element, DataNode *parent)
{
  DataGenericCameraSensor *sensor = new DataGenericCameraSensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateGenericAmbientLightSensor(DataParseElement *element, DataNode *parent)
{
  DataGenericAmbientLightSensor *sensor = new DataGenericAmbientLightSensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateGenericInternalEnergySensor(DataParseElement *element, DataNode *parent)
{
  DataGenericInternalEnergySensor *sensor = new DataGenericInternalEnergySensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateGenericOrientationSensor(DataParseElement *element, DataNode *parent)
{
  DataGenericOrientationSensor *sensor = new DataGenericOrientationSensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateGenericPositionSensor(DataParseElement *element, DataNode *parent)
{
  DataGenericPositionSensor *sensor = new DataGenericPositionSensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateGenericLDRSensor(DataParseElement *element,
    DataNode *parent)
{
  DataGenericLightDependentResistorSensor *sensor = new DataGenericLightDependentResistorSensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateSharpDM2Y3A003K0FSensor(DataParseElement *element, DataNode *parent)
{
  DataSharpDM2Y3A003K0FSensor *sensor = new DataSharpDM2Y3A003K0FSensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateSharpGP2D12_37Sensor(DataParseElement *element, DataNode *parent)
{
  DataSharpGP2D12_37Sensor *sensor = new DataSharpGP2D12_37Sensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateGenericFeedbackSensor(DataParseElement *element, DataNode *parent)
{
  DataGenericFeedbackSensor *sensor = new DataGenericFeedbackSensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateGenericVelocitySensor(DataParseElement *element, DataNode *parent)
{
  DataGenericVelocitySensor *sensor = new DataGenericVelocitySensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateGenericDeflectionSensor(DataParseElement *element, DataNode *parent)
{
  DataGenericDeflectionSensor *sensor = new DataGenericDeflectionSensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateGenericSignalSensor(DataParseElement *element, DataNode *parent)
{
  DataGenericSignalSensor *sensor = new DataGenericSignalSensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateGenericBinaryContactSensor(DataParseElement *element, DataNode *parent)
{
  DataGenericBinaryContactSensor *sensor = new DataGenericBinaryContactSensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateGenericActuatorSensor(DataParseElement *element, DataNode *parent)
{
  DataGenericActuatorSensor *sensor = new DataGenericActuatorSensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateObjectVelocitySensor(DataParseElement *element, DataNode *parent)
{
  DataObjectVelocitySensor *sensor = new DataObjectVelocitySensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

DataSensor* DataSensorFactory::__generateObjectAngularVelocitySensor(DataParseElement *element, DataNode *parent)
{
  DataObjectAngularVelocitySensor *sensor = new DataObjectAngularVelocitySensor(parent);
  sensor->add(element);
  return (DataSensor*)sensor;
}

void DataSensorFactory::createXsd(XsdSpecification *spec)
{
  XsdChoice *sensorListDefinition = new XsdChoice(YARS_STRING_SENSOR_LIST_DEFINITION,         "0", YARS_STRING_XSD_UNBOUNDED);
  sensorListDefinition->add(NE(YARS_STRING_GENERIC_PROXIMITY,
                               YARS_STRING_GENERIC_PROXIMITY_DEFINITION,                      "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_GENERIC_CAMERA_SENSOR,
                               YARS_STRING_GENERIC_CAMERA_SENSOR_DEFINITION,                  "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_GENERIC_AMBIENT_LIGHT_SENSOR,
                               YARS_STRING_GENERIC_AMBIENT_LIGHT_SENSOR_DEFINITION,           "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_GENERIC_INTERNAL_ENERGY_SENSOR,
                               YARS_STRING_GENERIC_INTERNAL_ENERGY_SENSOR_DEFINITION,         "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_GENERIC_ORIENTATION_SENSOR,
                               YARS_STRING_GENERIC_ORIENTATION_SENSOR_DEFINITION,             "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_GENERIC_POSITION_SENSOR,
                               YARS_STRING_GENERIC_POSITION_SENSOR_DEFINITION,                "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_GENERIC_LDR_SENSOR,
                               YARS_STRING_GENERIC_LDR_SENSOR_DEFINITION,                     "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_SHARP_IR_DM2Y3A003K0F,
                               YARS_STRING_SHARP_IR_DM2Y3A003K0F_DEFINITION,                  "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_SHARP_IR_GP2D12_37,
                               YARS_STRING_SHARP_IR_GP2D12_37_DEFINITION,                     "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_GENERIC_FEEDBACK_SENSOR,
                               YARS_STRING_GENERIC_FEEDBACK_SENSOR_DEFINITION,                "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_GENERIC_VELOCITY_SENSOR,
                               YARS_STRING_GENERIC_VELOCITY_SENSOR_DEFINITION,                "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_GENERIC_DEFLECTION_SENSOR,
                               YARS_STRING_GENERIC_DEFLECTION_SENSOR_DEFINITION,              "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_GENERIC_SIGNAL_SENSOR,
                               YARS_STRING_GENERIC_SIGNAL_SENSOR_DEFINITION,                  "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_GENERIC_BINARY_CONTACT_SENSOR,
                               YARS_STRING_GENERIC_BINARY_CONTACT_SENSOR_DEFINITION,          "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_GENERIC_ACTUATOR_SENSOR,
                               YARS_STRING_GENERIC_ACTUATOR_SENSOR_DEFINITION,                "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_GENERIC_OBJECT_VELOCITY_SENSOR,
                               YARS_STRING_GENERIC_OBJECT_VELOCITY_SENSOR_DEFINITION,         "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_GENERIC_OBJECT_ANGULAR_VELOCITY_SENSOR,
                               YARS_STRING_GENERIC_OBJECT_ANGULAR_VELOCITY_SENSOR_DEFINITION, "0", YARS_STRING_XSD_UNBOUNDED));
  sensorListDefinition->add(NE(YARS_STRING_MUSCLE_SENSOR,
                               YARS_STRING_MUSCLE_SENSOR_DEFINITION, "0", YARS_STRING_XSD_UNBOUNDED));
  spec->add(sensorListDefinition);

  DataGenericProximitySensor::createXsd(spec);
  DataGenericCameraSensor::createXsd(spec);
  DataGenericAmbientLightSensor::createXsd(spec);
  DataGenericInternalEnergySensor::createXsd(spec);
  DataGenericOrientationSensor::createXsd(spec);
  DataGenericLightDependentResistorSensor::createXsd(spec);
  DataSharpDM2Y3A003K0FSensor::createXsd(spec);
  DataSharpGP2D12_37Sensor::createXsd(spec);
  DataGenericFeedbackSensor::createXsd(spec);
  DataGenericVelocitySensor::createXsd(spec);
  DataGenericDeflectionSensor::createXsd(spec);
  DataGenericSignalSensor::createXsd(spec);
  DataGenericBinaryContactSensor::createXsd(spec);
  DataGenericPositionSensor::createXsd(spec);
  DataGenericActuatorSensor::createXsd(spec);
  DataObjectVelocitySensor::createXsd(spec);
  DataObjectAngularVelocitySensor::createXsd(spec);
  DataMuscleSensor::createXsd(spec);
}
