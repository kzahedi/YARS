#include "DataActuatorFactory.h"

#include "DataNode.h"
#include "DataParseElement.h"
#include "DataActuator.h"
#include "DataHingeActuator.h"
#include "DataSliderActuator.h"
#include "DataFixedActuator.h"
#include "DataGenericActuator.h"
#include "DataMuscleActuator.h"

#include <sstream>

using namespace std;

# define GENERIC             "generic"
# define YARS_STRING_GENERIC (char*)GENERIC

DataActuator* DataActuatorFactory::actuator(DataParseElement *element, DataNode *parent)
{
  if(element->opening(YARS_STRING_HINGE))   return __generateHingeActuator(element,   parent);
  if(element->opening(YARS_STRING_SLIDER))  return __generateSliderActuator(element,  parent);
  if(element->opening(YARS_STRING_FIXED))   return __generateFixedActuator(element,   parent);
  if(element->opening(YARS_STRING_GENERIC)) return __generateGenericActuator(element, parent);
  if(element->opening(YARS_STRING_MUSCLE))
    return __generateGenericActuator(element, parent);
  return NULL;
}

DataHingeActuator* DataActuatorFactory::__generateHingeActuator(DataParseElement *element, DataNode *parent)
{
  DataHingeActuator *actuator = new DataHingeActuator(parent);
  actuator->add(element);
  return actuator;
}

DataSliderActuator* DataActuatorFactory::__generateSliderActuator(DataParseElement *element, DataNode *parent)
{
  DataSliderActuator *actuator = new DataSliderActuator(parent);
  actuator->add(element);
  return actuator;
}

DataFixedActuator* DataActuatorFactory::__generateFixedActuator(DataParseElement *element, DataNode *parent)
{
  DataFixedActuator *actuator = new DataFixedActuator(parent);
  actuator->add(element);
  return actuator;
}

DataGenericActuator* DataActuatorFactory::__generateGenericActuator(DataParseElement *element, DataNode *parent)
{
  DataGenericActuator *actuator = new DataGenericActuator(parent);
  actuator->add(element);
  return actuator;
}

DataMuscleActuator* DataActuatorFactory::__generateMuscleActuator(
    DataParseElement *element, DataNode *parent)
{
  auto actuator = new DataMuscleActuator(parent);
  actuator->add(element);
  return actuator;
}

void DataActuatorFactory::createXsd(XsdSpecification *spec)
{
  XsdChoice *actuatorListDefinition = new XsdChoice(YARS_STRING_ACTUATOR_LIST_DEFINITION, "1", YARS_STRING_XSD_UNBOUNDED);
  actuatorListDefinition->add(NE(YARS_STRING_HINGE,   YARS_STRING_HINGE_DEFINITION,   0 ,1));
  actuatorListDefinition->add(NE(YARS_STRING_SLIDER,  YARS_STRING_SLIDER_DEFINITION,  0 ,1));
  actuatorListDefinition->add(NE(YARS_STRING_FIXED,   YARS_STRING_FIXED_DEFINITION,   0 ,1));
  actuatorListDefinition->add(NE(YARS_STRING_GENERIC, YARS_STRING_GENERIC_DEFINITION, 0 ,1));
  actuatorListDefinition->add(NE(YARS_STRING_MUSCLE,
    YARS_STRING_MUSCLE_DEFINITION, 0 ,1));
  spec->add(actuatorListDefinition);
  stringstream comment;
  comment << "The list of actuators (joints, motors, ...)." << endl;
  comment << "  The order given here, determines the order" << endl;
  comment << "  in which the actuators are communicated over" << endl;
  comment << "  the communication port, and in which order" << endl;
  comment << "  they are indexed in the RobotController." << endl;
  actuatorListDefinition->setComment(comment.str());

  DataHingeActuator::createXsd(spec);
  DataSliderActuator::createXsd(spec);
  DataFixedActuator::createXsd(spec);
  DataGenericActuator::createXsd(spec);
  DataMuscleActuator::createXsd(*spec);
}
