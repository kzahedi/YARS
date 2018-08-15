#ifndef __DATA_ACTUATOR_FACTOR_H__
#define __DATA_ACTUATOR_FACTOR_H__

#include "DataNode.h"
#include "DataParseElement.h"
#include "DataActuator.h"
#include "DataHingeActuator.h"
#include "DataSliderActuator.h"
#include "DataMuscleActuator.h"
#include "DataFixedActuator.h"
#include "DataGenericActuator.h"

# define YARS_STRING_ACTUATOR_LIST            (char*)"actuators"
# define YARS_STRING_ACTUATOR_LIST_DEFINITION (char*)"actuators_divider_definition"

class DataActuatorFactory
{
  public:
    static DataActuator* actuator(DataParseElement *element, DataNode *parent);
    static void createXsd(XsdSpecification *spec);

  private:
    static DataHingeActuator*   __generateHingeActuator(DataParseElement   *element, DataNode *parent);
    static DataSliderActuator*  __generateSliderActuator(DataParseElement  *element, DataNode *parent);
    static DataMuscleActuator*  __generateMuscleActuator(DataParseElement  *element, DataNode *parent);
    static DataFixedActuator*   __generateFixedActuator(DataParseElement   *element, DataNode *parent);
    static DataGenericActuator* __generateGenericActuator(DataParseElement *element, DataNode *parent);
};

#endif // __DATA_ACTUATOR_FACTOR_H__
