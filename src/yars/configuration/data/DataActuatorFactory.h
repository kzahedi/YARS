#ifndef __DATA_ACTUATOR_FACTOR_H__
#define __DATA_ACTUATOR_FACTOR_H__

#include "DataNode.h"
#include "DataParseElement.h"
#include "DataActuator.h"
#include "DataHingeActuator.h"
#include "DataSliderActuator.h"
#include "DataFixedActuator.h"
#include "DataGenericActuator.h"

# define ACTUATORS                            "actuators"
# define YARS_STRING_ACTUATOR_LIST            (char*)ACTUATORS
# define YARS_STRING_ACTUATOR_LIST_DEFINITION (char*)ACTUATORS DIVIDER DEFINITION

class DataActuatorFactory
{
  public:
    static DataActuator* actuator(DataParseElement *element, DataNode *parent);
    static void createXsd(XsdSpecification *spec);

  private:
    static DataHingeActuator*   __generateHingeActuator(DataParseElement   *element, DataNode *parent);
    static DataSliderActuator*  __generateSliderActuator(DataParseElement  *element, DataNode *parent);
    static DataFixedActuator*   __generateFixedActuator(DataParseElement   *element, DataNode *parent);
    static DataGenericActuator* __generateGenericActuator(DataParseElement *element, DataNode *parent);
};

#endif // __DATA_ACTUATOR_FACTOR_H__
