#include "ActuatorFactory.h"
#include "HingeActuator.h"
#include "SliderActuator.h"
#include "FixedActuator.h"
#include "MuscleActuator.h"
#include "GenericActuator.h"
#include "GenericSpringActuator.h"

Actuator* ActuatorFactory::create(DataActuator *actuator, Robot *robot)
{
  switch(actuator->type())
  {
    case DATA_ACTUATOR_HINGE:   return __createHinge(actuator,   robot); break;
    case DATA_ACTUATOR_SLIDER:  return __createSlider(actuator,  robot); break;
    case DATA_ACTUATOR_FIXED:   return __createFixed(actuator,   robot); break;
    case DATA_ACTUATOR_GENERIC: return __createGeneric(actuator, robot); break;
    case DATA_ACTUATOR_MUSCLE:  return __createMuscle(actuator, robot); break;
  }
  YarsErrorHandler::push("Actuator error. Unknown type given.");
  return NULL;
}

Actuator* ActuatorFactory::__createMuscle(DataActuator *actuator, Robot *robot)
{
  DataMuscleActuator *data = (DataMuscleActuator*)actuator;
  MuscleActuator *a        = new MuscleActuator(data, robot);
  return a;
}

Actuator* ActuatorFactory::__createHinge(DataActuator *actuator, Robot *robot)
{
  DataHingeActuator *data = (DataHingeActuator*)actuator;
  HingeActuator *a        = new HingeActuator(data, robot);
  return a;
}

Actuator* ActuatorFactory::__createSlider(DataActuator *actuator, Robot *robot)
{
  DataSliderActuator *data = (DataSliderActuator*)actuator;
  SliderActuator *a        = new SliderActuator(data, robot);
  return a;
}

Actuator* ActuatorFactory::__createFixed(DataActuator *actuator, Robot *robot)
{
  DataFixedActuator *data = (DataFixedActuator*)actuator;
  FixedActuator *a        = new FixedActuator(data, robot);
  return a;
}

Actuator* ActuatorFactory::__createGeneric(DataActuator *actuator, Robot *robot)
{
  DataGenericActuator *data = (DataGenericActuator*)actuator;
  if(data->usesSprings())
  {
    GenericSpringActuator *a = new GenericSpringActuator(data, robot);
    return (Actuator*)a;
  }
  else
  {
    GenericActuator *a = new GenericActuator(data, robot);
    return (Actuator*)a;
  }
  YarsErrorHandler::push("Unknown generic actuator selected");
  return NULL;
}
