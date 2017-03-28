#include "DataMuscleActuator.h"

#include "yars/configuration/data/DataActuator.h"
#include "yars/configuration/xsd/specification/XsdSpecification.h"
#include "yars/util/YarsErrorHandler.h"
#include "yars/view/console/ConsoleView.h"

using namespace std;

// TODO: check which definitions can be put to higher level in class hierarchy.
#define YARS_STRING_VELOCITY (char*)"velocity"
#define YARS_STRING_FORCE (char*)"force"
#define YARS_STRING_FORCE_DEFINITION (char*)"actuator_force" DIVIDER DEFINITION
#define YARS_STRING_FORCE_VELOCITY (char*)"force and velocity"
#define YARS_STRING_SCALING (char*)"scaling"
#define YARS_STRING_MAXIMUM (char*)"max"
#define YARS_STRING_ACTUATOR_TYPE_DEFINITION (char*)"actuator" DIVIDER "type" \
  DIVIDER "positional" DIVIDER "velocity" DIVIDER "force" DIVIDER DEFINITION
#define YARS_STRING_FORCE_LENGTH_MODEL (char*)"force-length" DIVIDER "model"
#define YARS_STRING_FORCE_LENGTH_MODEL_DEFINITION (char*)"force-length" \
  DIVIDER "model" DIVIDER DEFINITION
#define YARS_STRING_FORCE_VELOCITY_MODEL (char*)"force-velocity" DIVIDER "model"
#define YARS_STRING_FORCE_VELOCITY_MODEL_DEFINITION (char*)"force-velocity" \
  DIVIDER "model" DIVIDER DEFINITION

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

  _maxVelocity = 0.0;
  _maxForce = 0.0;
  _forceScaling = -1.0;
}

yReal DataMuscleActuator::velocity() const
{
  return _maxVelocity;
}

yReal DataMuscleActuator::force() const
{
  return _maxForce;
}

void DataMuscleActuator::add(DataParseElement* element)
{
  cout << "adding elements from xml. current elem:" << element->name() << endl;
  if (element->closing(YARS_STRING_MUSCLE)) {
    close();
    current = parent;
  } else if (element->opening(YARS_STRING_MUSCLE)) {
    element->set(YARS_STRING_NAME, _name);
    element->set(YARS_STRING_TYPE, _jointType);
  } else if (element->opening(YARS_STRING_SOURCE)) {
    element->set(YARS_STRING_NAME, _source);
  } else if (element->opening(YARS_STRING_DESTINATION)) {
    element->set(YARS_STRING_NAME, _destination);
  } else if (element->opening(YARS_STRING_FORCE)) {
    element->set(YARS_STRING_MAXIMUM, _maxForce);
    element->set(YARS_STRING_SCALING, _forceScaling);
  } else if (element->opening(YARS_STRING_VELOCITY)) {
    element->set(YARS_STRING_MAXIMUM, _maxVelocity);
  }
}

void DataMuscleActuator::close()
{
  if (_jointType == YARS_STRING_FORCE_VELOCITY)
    _controlType = DATA_ACTUATOR_CONTROL_FORCE_VELOCITY;
  else
    cout << "Unkown _jointType: " << _jointType << endl;

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

  switch(_controlType) {
  case DATA_ACTUATOR_CONTROL_FORCE_VELOCITY:
    _internalDomain[0].min =  0.0;
    _internalDomain[0].max = _maxForce;
    _externalDomain[0] = _mapping;
    _internalExternalMapping[0].setInputDomain(_internalDomain[0]);
    _internalExternalMapping[0].setOutputDomain(_externalDomain[0]);

    _internalDomain[1].min = -_maxVelocity;
    _internalDomain[1].max = _maxVelocity;
    _externalDomain[1] = _mapping;
    cout << "setting velocity to " << _internalDomain[1] << " " << _externalDomain[1] << endl;
    _internalExternalMapping[1].setInputDomain(_internalDomain[1]);
    _internalExternalMapping[1].setOutputDomain(_externalDomain[1]);
    break;
  default: cout << __LINE__ << " Unkown _controlType: " << _controlType << endl;
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
  auto copy = new DataMuscleActuator(nullptr);

  copy->_mapping = _mapping;
  copy->_destination = _destination;
  copy->_jointType = _jointType;
  copy->_name = _name;
  copy->_source = _source;
  copy->_axisOrientation = _axisOrientation;
  copy->_axisPosition = _axisPosition;
  // Duplicate in DataActuator::copy. But necessarry for setMapping()
  copy->_controlType = _controlType; 
  copy->setMapping();

  return copy;
}

void DataMuscleActuator::createXsd(XsdSpecification& spec)
{
  // TODO: smart pointer?
  auto muscleDef = new XsdSequence(YARS_STRING_MUSCLE_DEFINITION);
  muscleDef->add(NA(YARS_STRING_NAME, YARS_STRING_XSD_STRING, false));
  muscleDef->add(NA(YARS_STRING_TYPE, YARS_STRING_ACTUATOR_TYPE_DEFINITION,
    false));
  muscleDef->add(NE(YARS_STRING_SOURCE, YARS_STRING_NAME_DEFINITION, 1, 1));
  muscleDef->add(NE(YARS_STRING_DESTINATION, YARS_STRING_NAME_DEFINITION, 1,
    1));
  muscleDef->add(NE(YARS_STRING_FORCE_LENGTH_MODEL,
    "muscle_model_definition", 1, 1));
  muscleDef->add(NE(YARS_STRING_FORCE_VELOCITY_MODEL,
    "muscle_model_definition", 1, 1));
  spec.add(muscleDef);

  auto actuatorTypeDef = new XsdEnumeration(
      YARS_STRING_ACTUATOR_TYPE_DEFINITION, YARS_STRING_XSD_STRING);
  actuatorTypeDef->add(YARS_STRING_FORCE_VELOCITY);
  spec.add(actuatorTypeDef);

  auto muscleModelDef = new XsdSequence("muscle_model_definition");
  muscleModelDef->add(NA("model", "muscle_type_definition", true));
  spec.add(muscleModelDef);

  auto muscleTypeDef = new XsdEnumeration("muscle_type_definition",
    YARS_STRING_XSD_STRING);
  muscleTypeDef->add("constant");
  muscleTypeDef->add("linear");
  muscleTypeDef->add("hill");
  spec.add(muscleTypeDef);
}
