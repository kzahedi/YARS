#include "DataMuscleActuator.h"

#include "yars/configuration/data/DataActuator.h"
#include "yars/configuration/xsd/specification/XsdSpecification.h"

#define YARS_STRING_MUSCLE_DEFINITION "slider" DIVIDER "definition"
#define YARS_STRING_ACTUATOR_MODE_DEFINITION "actuator" \
  DIVIDER "mode" DIVIDER "active" DIVIDER "passive" DIVIDER "definition"
#define YARS_STRING_FORCE_DEFINITION "actuator_force" DIVIDER "definition"

DataMuscleActuator::DataMuscleActuator(DataNode *parent)
  : DataActuator(parent, DATA_ACTUATOR_MUSCLE)
{
}

DataMuscleActuator::~DataMuscleActuator()
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
  XsdSequence *muscleDefinition = new XsdSequence(YARS_STRING_MUSCLE_DEFINITION);
  muscleDefinition->add(NA("name", "xs:string", false));
  muscleDefinition->add(NA("type", "xs:string", false));
  muscleDefinition->add(NA("mode", YARS_STRING_ACTUATOR_MODE_DEFINITION,
    true));
  muscleDefinition->add(NE("source",      YARS_STRING_NAME_DEFINITION, 1, 1));
  muscleDefinition->add(NE("destination", YARS_STRING_NAME_DEFINITION, 1, 1));
  muscleDefinition->add(NE("force-length_model",
    "force-length_model_definition", 1, 1));
  muscleDefinition->add(NE("force-velocity_model",
    "force-velocity_model_definition", 1, 1));
  spec.add(muscleDefinition);

  XsdEnumeration *forceLengthModelDef = new XsdEnumeration(
    "force-length_model_definition","xs:string");
  forceLengthModelDef->add("constant");
  forceLengthModelDef->add("linear");
  forceLengthModelDef->add("hill");
  spec.add(forceLengthModelDef);

  XsdEnumeration *forceVelocityModelDef = new XsdEnumeration(
    "force-length_model_definition","xs:string");
  forceVelocityModelDef->add("constant");
  forceVelocityModelDef->add("linear");
  forceVelocityModelDef->add("hill");
  spec.add(forceVelocityModelDef);
}
