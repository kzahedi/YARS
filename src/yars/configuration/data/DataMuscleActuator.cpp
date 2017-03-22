#include "DataMuscleActuator.h"

#include "yars/configuration/data/DataActuator.h"
#include "yars/configuration/xsd/specification/XsdSpecification.h"

#include <memory>

#define YARS_STRING_MUSCLE (char*)"muscle"
#define YARS_STRING_MUSCLE_DEFINITION YARS_STRING_MUSCLE DIVIDER "definition"

// TODO: check which definitions can be put to higher level in class hierarchy.
//#define YARS_STRING_ACTUATOR_MODE_DEFINITION "actuator" \
  DIVIDER "mode" DIVIDER "active" DIVIDER "passive" DIVIDER "definition"
#define YARS_STRING_FORCE_DEFINITION (char*)"actuator_force" DIVIDER \
  "definition"
#define YARS_STRING_FORCE_VELOCITY (char*)"force and velocity"

// Probably will need to be constexpr because of g. Think what to do with g
// then.
DataMuscleActuator::DataMuscleActuator(DataNode* parent)
  : DataActuator{parent, DATA_ACTUATOR_MUSCLE}
{
  _internalValue.resize(1);
  _externalValue.resize(1);
  _desiredValue.resize(1);
  _desiredExValue.resize(1);
  _internalExternalMapping.resize(1);
  _internalDomain.resize(1);
  _externalDomain.resize(1);

  _internalValue[0] = 0.0;
  _externalValue[0] = 0.0;
  _desiredValue[0] = 0.0;
  _desiredExValue[0] = 0.0;
}

void DataMuscleActuator::add(DataParseElement* element)
{
  if (element->closing(YARS_STRING_MUSCLE)) {
    close();
    current = parent;
  } else if (element->opening(YARS_STRING_MUSCLE)) {
    element->set(YARS_STRING_NAME, _name);
  } else if (element->opening(YARS_STRING_SOURCE)) {
    element->set(YARS_STRING_NAME, _source);
  } else if (element->opening(YARS_STRING_DESTINATION)) {
    element->set(YARS_STRING_NAME, _destination);
  //} else if (element->opening(YARS_STRING_NOISE)) {
  } else {
  }
}

void DataMuscleActuator::close()
{
  if (_jointType == YARS_STRING_FORCE_VELOCITY)
    _controlType = DATA_ACTUATOR_CONTROL_FORCE_VELOCITY;

  setMapping();
}

void DataMuscleActuator::setMapping()
{
  // Assume force-velocity for now
  if (_controlType == DATA_ACTUATOR_CONTROL_FORCE_VELOCITY) {
    _internalValue.resize(2);
    _externalValue.resize(2);
    _desiredValue.resize(2);
    _desiredExValue.resize(2);
    _internalExternalMapping.resize(2);
    _internalDomain.resize(2);
    _externalDomain.resize(2);
  }
}

void DataMuscleActuator::applyOffset(Pose pose)
{
}

std::string DataMuscleActuator::source()
{
  return _source;
}

std::string DataMuscleActuator::destination() const
{
  return _destination;
}

std::string DataMuscleActuator::name()
{
  return _name;
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
  return 1;
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
  muscleDef->add(NA(YARS_STRING_NAME, YARS_STRING_XSD_STRING, false));
  muscleDef->add(NA(YARS_STRING_TYPE, YARS_STRING_XSD_STRING, false));
  //muscleDef->add(NA("mode", YARS_STRING_ACTUATOR_MODE_DEFINITION, true));
  muscleDef->add(NE("source",      YARS_STRING_NAME_DEFINITION, 1, 1));
  muscleDef->add(NE("destination", YARS_STRING_NAME_DEFINITION, 1, 1));
  muscleDef->add(NE("force-length model", "force-length_model_definition", 1,
    1));
  muscleDef->add(NE("force-velocity model", "force-velocity_model_definition",
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
