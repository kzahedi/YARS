#include "DataPointConstraint.h"

#include "yars/configuration/data/DataActuator.h"
#include "yars/configuration/data/DataPoseFactory.h"
#include "yars/configuration/xsd/specification/XsdSpecification.h"
#include "yars/util/YarsErrorHandler.h"
#include "yars/view/console/ConsoleView.h"
#include <yars/configuration/data/DataDomainFactory.h>
#include <yars/defines/mutex.h>

using namespace std;

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
#define YARS_STRING_FORCE_LENGTH_MODEL_DEFINITION (char*)"force-length" \
  DIVIDER "model" DIVIDER DEFINITION
#define YARS_STRING_FORCE_VELOCITY_MODEL (char*)"force-velocity" DIVIDER "model"
#define YARS_STRING_FORCE_VELOCITY_MODEL_DEFINITION (char*)"force-velocity" \
  DIVIDER "model" DIVIDER DEFINITION
#define YARS_STRING_MAPPING (char*)"mapping"
#define YARS_STRING_MIN_MAX_DEFINITION (char*)"min" DIVIDER "max" DIVIDER \
  DEFINITION

// For XSD Pose.
#define YARS_STRING_POSE (char*)"pose"
#define YARS_STRING_RAD_DEG_DEFINITION (char*)"radOrDeg" DIVIDER DEFINITION
#define YARS_STRING_GLOBAL (char*)"global"
#define YARS_STRING_POSEG_DEFINITION (char*)"pose_with_global" DIVIDER \
  DEFINITION

DataPointConstraint::DataPointConstraint(DataNode* parent)
  : DataActuator{parent, DATA_CONSTRAINT_POINT}
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

yReal DataPointConstraint::velocity() const
{
  return _maxVelocity;
}

yReal DataPointConstraint::force() const
{
  return _maxForce;
}

void DataPointConstraint::add(DataParseElement* element)
{
  cout << "adding elements from xml. current elem:" << element->name() << endl;
  if (element->closing(YARS_STRING_POINT_CONSTRAINT))
  {
    close();
    current = parent;
  }
  else if (element->opening(YARS_STRING_POINT_CONSTRAINT))
  {
    element->set(YARS_STRING_NAME, _name);
    element->set(YARS_STRING_TYPE, _jointType);
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

void DataPointConstraint::close()
{
  if (_jointType == YARS_STRING_FORCE_VELOCITY)
  {
    _controlType = DATA_ACTUATOR_CONTROL_FORCE_VELOCITY;
  }
  else
  {
    cout << "Unkown _jointType: " << _jointType << endl;
  }

  setMapping();
}

void DataPointConstraint::setMapping()
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

void DataPointConstraint::applyOffset(Pose offset)
{
  if(_poseInWorldCoordinates) return;
  _pose << offset;
  _axisPosition      = _pose.position;
  _axisOrientation   = _pose.orientation;
}

std::string DataPointConstraint::source()
{
  return _source;
}

std::string DataPointConstraint::destination() const
{
  return _destination;
}

std::string DataPointConstraint::name()
{
  return _name;
}

void DataPointConstraint::setInternalValue(int index, yReal value)
{
  YM_LOCK;
  _internalValue[index] = _internalDomain[index].cut(value);
  _externalValue[index] = _internalExternalMapping[index].map(_internalValue[index]);
  YM_UNLOCK;
}

void DataPointConstraint::setExternalValue(int index, yReal value)
{
  YM_LOCK;
  _externalValue[index] = _externalDomain[index].cut(value);
  _internalValue[index] = _internalExternalMapping[index].invMap(_externalValue[index]);
  YM_UNLOCK;
}

yReal DataPointConstraint::internalValue(int index)
{
  YM_LOCK;
  yReal r = _internalValue[index];
  YM_UNLOCK;
  return r;
}

yReal DataPointConstraint::externalValue(int index)
{
  YM_LOCK;
  yReal r = _externalValue[index];
  YM_UNLOCK;
  return r;
}

int DataPointConstraint::dimension()
{
  return 1;
}

void DataPointConstraint::setDesiredValue(int index, yReal value)
{
  YM_LOCK;
  cout << value << endl;
  _desiredExValue[index] = _externalDomain[index].cut(value);
  _desiredValue[index] = _internalExternalMapping[index].invMap(_desiredExValue[index]);
  YM_UNLOCK;
}

yReal DataPointConstraint::getInternalDesiredValue(int index)
{
  YM_LOCK;
  yReal r = _desiredValue[index];
  YM_UNLOCK;
  return r;
}

yReal DataPointConstraint::getExternalDesiredValue(int index)
{
  YM_LOCK;
  yReal r = _desiredExValue[index];
  YM_UNLOCK;
  return r;
}

Domain DataPointConstraint::getInternalDomain(int index)
{
  YM_LOCK;
  Domain r = _internalDomain[index];
  YM_UNLOCK;
  return r;
}

Domain DataPointConstraint::getExternalDomain(int index)
{
  YM_LOCK;
  Domain r = _externalDomain[index];
  YM_UNLOCK;
  return r;
}

bool DataPointConstraint::isActive(int index)
{
  return _isActive;
}

yReal DataPointConstraint::getAppliedForce(int index)
{
  return _appliedForce;
}

yReal DataPointConstraint::getAppliedVelocity(int index)
{
  return _appliedVelocity;
}

void DataPointConstraint::setAppliedForceAndVelocity(int index, yReal force,
                                                    yReal velocity)
{
  _appliedForce    = force;
  _appliedVelocity = velocity;
}

Pose DataPointConstraint::pose()
{
  YM_LOCK;
  Pose r = _pose;
  YM_UNLOCK;
  return r;
}

DataActuator* DataPointConstraint::_copy()
{
  auto copy = new DataPointConstraint(nullptr);

  copy->_mapping         = _mapping;
  copy->_pose               = _pose;
  copy->_destination     = _destination;
  copy->_jointType       = _jointType;
  copy->_name            = _name;
  copy->_source          = _source;
  copy->_maxForce        = _maxForce;
  copy->_maxVelocity     = _maxVelocity;
  copy->_axisOrientation = _axisOrientation;
  copy->_axisPosition    = _axisPosition;
  // Duplicate in DataActuator::copy. But necessarry for setMapping()
  copy->_controlType = _controlType;
  copy->setMapping();

  return copy;
}

void DataPointConstraint::createXsd(XsdSpecification& spec)
{
  // TODO: smart pointer?
  auto constraintDefinition = new XsdSequence(YARS_STRING_POINT_CONSTRAINT_DEFINITION);
  constraintDefinition->add(NA(YARS_STRING_NAME, YARS_STRING_XSD_STRING,
        false));
  constraintDefinition->add(NE(YARS_STRING_SOURCE, YARS_STRING_NAME_DEFINITION,
        1, 1));
  constraintDefinition->add(NE(YARS_STRING_DESTINATION,
        YARS_STRING_NAME_DEFINITION, 1, 1));
  constraintDefinition->add(NE(YARS_STRING_POSE, YARS_STRING_POSEG_DEFINITION,
        0, 1));
  spec.add(constraintDefinition);

  auto poseDef = new XsdSequence(YARS_STRING_POSEG_DEFINITION);
  poseDef->add(NA(YARS_STRING_X, YARS_STRING_XSD_DECIMAL, false));
  poseDef->add(NA(YARS_STRING_Y, YARS_STRING_XSD_DECIMAL, false));
  poseDef->add(NA(YARS_STRING_Z, YARS_STRING_XSD_DECIMAL, false));
  poseDef->add(NA(YARS_STRING_ALPHA, YARS_STRING_XSD_DECIMAL, false));
  poseDef->add(NA(YARS_STRING_BETA, YARS_STRING_XSD_DECIMAL, false));
  poseDef->add(NA(YARS_STRING_GAMMA, YARS_STRING_XSD_DECIMAL, false));
  poseDef->add(NA(YARS_STRING_TYPE, YARS_STRING_RAD_DEG_DEFINITION, false));
  poseDef->add(NA(YARS_STRING_GLOBAL, YARS_STRING_TRUE_FALSE_DEFINITION,
        false));
  spec.add(poseDef);
}
