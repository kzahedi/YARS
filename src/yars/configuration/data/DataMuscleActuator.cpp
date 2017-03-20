#include "DataMuscleActuator.h"

#include "yars/configuration/data/DataActuator.h"
#include "yars/configuration/xsd/specification/XsdSpecification.h"

#include <memory>

#define YARS_STRING_MUSCLE (char*)"muscle"
#define YARS_STRING_MUSCLE_DEFINITION "muscle" DIVIDER "definition"
#define YARS_STRING_ACTUATOR_MODE_DEFINITION "actuator" \
  DIVIDER "mode" DIVIDER "active" DIVIDER "passive" DIVIDER "definition"
#define YARS_STRING_FORCE_DEFINITION "actuator_force" DIVIDER "definition"

// Probably will need to be constexpr because of g. Think what to do with g
// then.
DataMuscleActuator::DataMuscleActuator(DataNode* parent)
  : DataActuator{parent, DATA_ACTUATOR_MUSCLE}
{

}

void DataMuscleActuator::add(DataParseElement* element)
{
  if (element->closing(YARS_STRING_MUSCLE)) {
    close();
    current = parent;
  } else if (element->opening(YARS_STRING_MUSCLE)) {
  } else {
  }
}

void DataMuscleActuator::close()
{
}

void DataMuscleActuator::applyOffset(Pose pose)
{
}

std::string DataMuscleActuator::source()
{
}

std::string DataMuscleActuator::destination() const
{
  return _destination;
}

std::string DataMuscleActuator::name()
{
}

void DataMuscleActuator::setInternalValue(int index, yReal value)
{
}

void DataMuscleActuator::setExternalValue(int index, yReal value)
{
}

yReal DataMuscleActuator::internalValue(int index)
{
}

yReal DataMuscleActuator::externalValue(int index)
{
}

int DataMuscleActuator::dimension()
{
}

void DataMuscleActuator::setDesiredValue(int index, yReal value)
{
}

yReal DataMuscleActuator::getInternalDesiredValue(int index)
{
}

yReal DataMuscleActuator::getExternalDesiredValue(int index)
{
}

Domain DataMuscleActuator::getInternalDomain(int index)
{
}

Domain DataMuscleActuator::getExternalDomain(int index)
{
}

bool DataMuscleActuator::isActive(int index)
{
}

yReal DataMuscleActuator::getAppliedForce(int index)
{
}

yReal DataMuscleActuator::getAppliedVelocity(int index)
{
}

void DataMuscleActuator::setAppliedForceAndVelocity(int index, yReal force,
                                                    yReal velocity)
{
}

Pose DataMuscleActuator::pose()
{
}

DataActuator* DataMuscleActuator::_copy()
{
}

void DataMuscleActuator::createXsd(XsdSpecification& spec)
{
  // TODO: smart pointer?
  auto muscleDef = new XsdSequence(YARS_STRING_MUSCLE_DEFINITION);
  muscleDef->add(NA("name", "xs:string", false));
  muscleDef->add(NA("type", "xs:string", false));
  muscleDef->add(NA("mode", YARS_STRING_ACTUATOR_MODE_DEFINITION, true));
  muscleDef->add(NE("source",      YARS_STRING_NAME_DEFINITION, 1, 1));
  muscleDef->add(NE("destination", YARS_STRING_NAME_DEFINITION, 1, 1));
  muscleDef->add(NE("force-length_model", "force-length_model_definition", 1,
    1));
  muscleDef->add(NE("force-velocity_model", "force-velocity_model_definition",
        1, 1));
  spec.add(muscleDef);

  auto forceLengthModelDef = new XsdEnumeration(
    "force-length_model_definition","xs:string");
  forceLengthModelDef->add("constant");
  forceLengthModelDef->add("linear");
  forceLengthModelDef->add("hill");
  spec.add(forceLengthModelDef);

  auto forceVelocityModelDef = new XsdEnumeration(
    "force-length_model_definition","xs:string");
  forceVelocityModelDef->add("constant");
  forceVelocityModelDef->add("linear");
  forceVelocityModelDef->add("hill");
  spec.add(forceVelocityModelDef);
}
