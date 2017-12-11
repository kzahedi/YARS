#include "DataMuscleActuator.h"

#include "yars/configuration/data/DataActuator.h"
#include "yars/configuration/data/DataPoseFactory.h"
#include "yars/configuration/xsd/specification/XsdSpecification.h"
#include "yars/util/Mapping.h"
#include "yars/util/YarsErrorHandler.h"
#include "yars/view/console/ConsoleView.h"
#include <yars/configuration/data/DataDomainFactory.h>
#include <yars/defines/mutex.h>

// TODO: Remove pose. Positions for this constraint are generated automatically.

using namespace std;

// TODO: check which definitions can be put to higher level in class hierarchy.
#define YARS_STRING_VELOCITY (char*)"velocity"
#define YARS_STRING_VELOCITY_DEFINITION (char*)"slider_velocity" DIVIDER \
  DEFINITION
#define YARS_STRING_FORCE (char*)"force"
#define YARS_STRING_FORCE_DEFINITION (char*)"actuator_force" DIVIDER DEFINITION
#define YARS_STRING_FORCE_VELOCITY (char*)"force and velocity"
#define YARS_STRING_SCALING (char*)"scaling"
#define YARS_STRING_MAXIMUM (char*)"max"
#define YARS_STRING_ACTUATOR_TYPE_DEFINITION (char*)"actuator" DIVIDER "type" \
  DIVIDER "positional" DIVIDER "velocity" DIVIDER "force" DIVIDER DEFINITION
#define YARS_STRING_FORCE_LENGTH_MODEL (char*)"force-length" DIVIDER "model"
#define YARS_STRING_FORCE_VELOCITY_MODEL (char*)"force-velocity" DIVIDER "model"
#define YARS_STRING_MAPPING (char*)"mapping"
#define YARS_STRING_MIN_MAX_DEFINITION (char*)"min" DIVIDER "max" DIVIDER \
  DEFINITION

// For XSD Pose.
#define YARS_STRING_POSE (char*)"pose"
#define YARS_STRING_RAD_DEG_DEFINITION (char*)"radOrDeg" DIVIDER DEFINITION
#define YARS_STRING_GLOBAL (char*)"global"
#define YARS_STRING_POSEG_DEFINITION (char*)"pose_with_global" DIVIDER \
  DEFINITION

// Probably will need to be constexpr because of g. Think what to do with g
// then.
DataMuscleActuator::DataMuscleActuator(DataNode* parent)
  : DataActuator{parent, DATA_ACTUATOR_MUSCLE}
{
  _isActive = true;
  _poseInWorldCoordinates = false;

  _internalValue.resize(1);
  _externalValue.resize(1);
  _desiredValue.resize(1);
  _desiredExValue.resize(1);
  _internalExternalMapping.resize(1);
  _internalDomain.resize(1);
  _externalDomain.resize(1);

  _internalValue[0]  = 0.0;
  _externalValue[0]  = 0.0;
  _desiredValue[0]   = 0.0;
  _desiredExValue[0] = 0.0;

  _maxVelocity       = 0.0;
  _maxForce          = 0.0;
  _forceScaling      = -1.0;
  YM_INIT;
}

DataMuscleActuator::~DataMuscleActuator()
{
  YM_CLOSE;
}

double DataMuscleActuator::velocity() const
{
  return _maxVelocity;
}

double DataMuscleActuator::force() const
{
  return _maxForce;
}

void DataMuscleActuator::add(DataParseElement* element)
{
  cout << "adding elements from xml. current elem:" << element->name() << endl;
  if (element->closing(YARS_STRING_MUSCLE))
  {
    close();
    current = parent;
  }
  else if (element->opening(YARS_STRING_MUSCLE))
  {
    element->set(YARS_STRING_NAME, _name);
  }
  else if (element->opening(YARS_STRING_SOURCE))
  {
    element->set(YARS_STRING_NAME, _source);
  }
  else if (element->opening(YARS_STRING_DESTINATION))
  {
    element->set(YARS_STRING_NAME, _destination);
  }
  else if (element->opening(YARS_STRING_FORCE))
  {
    element->set(YARS_STRING_MAXIMUM, _maxForce);
    element->set(YARS_STRING_SCALING, _forceScaling);
  }
  else if (element->opening(YARS_STRING_VELOCITY))
  {
    element->set(YARS_STRING_MAXIMUM, _maxVelocity);
  }
  else if (element->opening(YARS_STRING_POSE))
  {
    DataPoseFactory::set(_pose, element);
    element->set(YARS_STRING_GLOBAL, _poseInWorldCoordinates);
    _axisPosition    = _pose.position;
    _axisOrientation = _pose.orientation;
  }
  else if (element->opening(YARS_STRING_MAPPING))
  {
    DataDomainFactory::set(_mapping, element);
  }
}

void DataMuscleActuator::close()
{
  setMapping();
}

void DataMuscleActuator::setMapping()
{
  _internalValue.resize(1);
  _externalValue.resize(1);
  _desiredValue.resize(1);
  _desiredExValue.resize(1);
  _internalExternalMapping.resize(1);
  _internalDomain.resize(1);
  _externalDomain.resize(1);
  _internalDomain[0].min =  -1.0; // A(t)
  _internalDomain[0].max =  1.0;
  // cout << "setting force to " << _internalDomain[0] << " " << _externalDomain[0] << endl;
  _externalDomain[0] = _mapping;
  _internalExternalMapping[0].setInputDomain(_internalDomain[0]);
  _internalExternalMapping[0].setOutputDomain(_externalDomain[0]);
}

void DataMuscleActuator::applyOffset(Pose offset)
{
  if(_poseInWorldCoordinates) return;
  _pose << offset;
  _axisPosition      = _pose.position;
  _axisOrientation   = _pose.orientation;
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

void DataMuscleActuator::setInternalValue(int index, double value)
{
  YM_LOCK;
  _internalValue[index] = _internalDomain[index].cut(value);
  _externalValue[index] = _internalExternalMapping[index].map(_internalValue[index]);
  YM_UNLOCK;
}

void DataMuscleActuator::setExternalValue(int index, double value)
{
  YM_LOCK;
  _externalValue[index] = _externalDomain[index].cut(value);
  _internalValue[index] = _internalExternalMapping[index].invMap(_externalValue[index]);
  YM_UNLOCK;
}

double DataMuscleActuator::internalValue(int index)
{
  YM_LOCK;
  double r = _internalValue[index];
  YM_UNLOCK;
  return r;
}

double DataMuscleActuator::externalValue(int index)
{
  YM_LOCK;
  double r = _externalValue[index];
  YM_UNLOCK;
  return r;
}

int DataMuscleActuator::dimension()
{
  return 1;
}

void DataMuscleActuator::setDesiredValue(int index, double value)
{
  YM_LOCK;
//  cout << value << endl;
  _desiredExValue[index] = _externalDomain[index].cut(value);
  _desiredValue[index] = _internalExternalMapping[index].invMap(_desiredExValue[index]);
  YM_UNLOCK;
}

double DataMuscleActuator::getInternalDesiredValue(int index)
{
  YM_LOCK;
  double r = _desiredValue[index];
  YM_UNLOCK;
  cout << "internal: " << r << endl;
  return r;
}

double DataMuscleActuator::getExternalDesiredValue(int index)
{
  YM_LOCK;
  double r = _desiredExValue[index];
  YM_UNLOCK;
  cout << "external: " << r << endl;
  return r;
}

Domain DataMuscleActuator::getInternalDomain(int index)
{
  YM_LOCK;
  Domain r = _internalDomain[index];
  YM_UNLOCK;
  return r;
}

Domain DataMuscleActuator::getExternalDomain(int index)
{
  YM_LOCK;
  Domain r = _externalDomain[index];
  YM_UNLOCK;
  return r;
}

bool DataMuscleActuator::isActive(int index)
{
  return _isActive;
}

double DataMuscleActuator::getAppliedForce(int index)
{
  return _appliedForce;
}

double DataMuscleActuator::getAppliedVelocity(int index)
{
  return _appliedVelocity;
}

void DataMuscleActuator::setAppliedForceAndVelocity(int index, double force,
                                                    double velocity)
{
  _appliedForce    = force;
  _appliedVelocity = velocity;
}

Pose DataMuscleActuator::pose()
{
  YM_LOCK;
  Pose r = _pose;
  YM_UNLOCK;
  return r;
}

DataActuator* DataMuscleActuator::_copy()
{
  auto copy = new DataMuscleActuator(nullptr);

  copy->_mapping         = _mapping;
  copy->_pose            = _pose;
  copy->_destination     = _destination;
  copy->_name            = _name;
  copy->_source          = _source;
  copy->_maxForce        = _maxForce;
  copy->_maxVelocity     = _maxVelocity;
  copy->_axisOrientation = _axisOrientation;
  copy->_axisPosition    = _axisPosition;

  copy->_controlType = _controlType;
  copy->setMapping();

  return copy;
}

void DataMuscleActuator::createXsd(XsdSpecification& spec)
{
  auto muscleDef = new XsdSequence(YARS_STRING_MUSCLE_DEFINITION);
  muscleDef->add(NA(YARS_STRING_NAME, YARS_STRING_XSD_STRING, false));
  muscleDef->add(NE(YARS_STRING_SOURCE, YARS_STRING_NAME_DEFINITION, 1, 1));
  muscleDef->add(NE(YARS_STRING_DESTINATION, YARS_STRING_NAME_DEFINITION, 1,
                    1));
  muscleDef->add(NE(YARS_STRING_FORCE, YARS_STRING_FORCE_DEFINITION, 1, 1));
  muscleDef->add(NE(YARS_STRING_VELOCITY, YARS_STRING_VELOCITY_DEFINITION, 1,
                    1));
  muscleDef->add(NE(YARS_STRING_POSE, YARS_STRING_POSEG_DEFINITION, 0, 1));
  muscleDef->add(NE(YARS_STRING_MAPPING, YARS_STRING_MIN_MAX_DEFINITION, 0, 1));
  muscleDef->add(NE(YARS_STRING_FORCE_LENGTH_MODEL, "muscle_model_definition",
                    1, 1));
  muscleDef->add(NE(YARS_STRING_FORCE_VELOCITY_MODEL, "muscle_model_definition",
                    1, 1));
  spec.add(muscleDef);

  auto muscleModelDef = new XsdSequence("muscle_model_definition");
  muscleModelDef->add(NA("model", "muscle_type_definition", true));
  spec.add(muscleModelDef);

  auto muscleTypeDef = new XsdEnumeration("muscle_type_definition",
                                          YARS_STRING_XSD_STRING);
  muscleTypeDef->add("constant");
  muscleTypeDef->add("linear");
  muscleTypeDef->add("hill");
  spec.add(muscleTypeDef);

  auto poseDef = new XsdSequence(YARS_STRING_POSEG_DEFINITION);
  poseDef->add(NA(YARS_STRING_X, YARS_STRING_XSD_DECIMAL, false));
  poseDef->add(NA(YARS_STRING_Y, YARS_STRING_XSD_DECIMAL, false));
  poseDef->add(NA(YARS_STRING_Z, YARS_STRING_XSD_DECIMAL, false));
  poseDef->add(NA(YARS_STRING_ALPHA, YARS_STRING_XSD_DECIMAL, false));
  poseDef->add(NA(YARS_STRING_BETA, YARS_STRING_XSD_DECIMAL, false));
  poseDef->add(NA(YARS_STRING_GAMMA, YARS_STRING_XSD_DECIMAL, false));
  poseDef->add(NA(YARS_STRING_TYPE,  YARS_STRING_RAD_DEG_DEFINITION, false));
  poseDef->add(NA(YARS_STRING_GLOBAL, YARS_STRING_TRUE_FALSE_DEFINITION,
                  false));
  spec.add(poseDef);
}
