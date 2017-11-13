#include "DataPointConstraint.h"

#include "yars/configuration/data/DataActuator.h"
#include "yars/configuration/data/DataPoseFactory.h"
#include "yars/configuration/xsd/specification/XsdSpecification.h"
#include "yars/util/Mapping.h"
#include "yars/util/YarsErrorHandler.h"
#include "yars/view/console/ConsoleView.h"
#include <yars/configuration/data/DataDomainFactory.h>
#include <yars/defines/mutex.h>

using namespace std;

#define YARS_STRING_MODE (char*)"mode"

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

  _forceScaling      = -1.0;
  YM_INIT;
}

DataPointConstraint::~DataPointConstraint()
{
  YM_CLOSE;
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
    element->set(YARS_STRING_MODE, _mode);
  }
  else if (element->opening(YARS_STRING_SOURCE))
  {
    element->set(YARS_STRING_NAME, _source);
  }
  else if (element->opening(YARS_STRING_DESTINATION))
  {
    element->set(YARS_STRING_NAME, _destination);
  }
  else if (element->opening(YARS_STRING_POSE))
  {
    DataPoseFactory::set(_pose, element);
    element->set(YARS_STRING_GLOBAL, _poseInWorldCoordinates);
    _axisPosition    = _pose.position;
    _axisOrientation = _pose.orientation;
  }
}

void DataPointConstraint::close()
{
}

/*!
 * In this case we always want the local coordinates. Therefore, we do nothing
 * in this function.
 */
void DataPointConstraint::applyOffset(Pose offset)
{
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

void DataPointConstraint::setInternalValue(int index, double value)
{
  YM_LOCK;
  _internalValue[index] = _internalDomain[index].cut(value);
  _externalValue[index] = _internalExternalMapping[index].map(_internalValue[index]);
  YM_UNLOCK;
}

void DataPointConstraint::setExternalValue(int index, double value)
{
  YM_LOCK;
  _externalValue[index] = _externalDomain[index].cut(value);
  _internalValue[index] = _internalExternalMapping[index].invMap(_externalValue[index]);
  YM_UNLOCK;
}

double DataPointConstraint::internalValue(int index)
{
  YM_LOCK;
  double r = _internalValue[index];
  YM_UNLOCK;
  return r;
}

double DataPointConstraint::externalValue(int index)
{
  YM_LOCK;
  double r = _externalValue[index];
  YM_UNLOCK;
  return r;
}

int DataPointConstraint::dimension()
{
  return 1;
}

void DataPointConstraint::setDesiredValue(int index, double value)
{
  YM_LOCK;
  _desiredExValue[index] = _externalDomain[index].cut(value);
  _desiredValue[index] = _internalExternalMapping[index].invMap(_desiredExValue[index]);
  YM_UNLOCK;
}

double DataPointConstraint::getInternalDesiredValue(int index)
{
  YM_LOCK;
  double r = _desiredValue[index];
  YM_UNLOCK;
  return r;
}

double DataPointConstraint::getExternalDesiredValue(int index)
{
  YM_LOCK;
  double r = _desiredExValue[index];
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

double DataPointConstraint::getAppliedForce(int index)
{
  return _appliedForce;
}

double DataPointConstraint::getAppliedVelocity(int index)
{
  return _appliedVelocity;
}

std::string DataPointConstraint::mode() const
{
  return _mode;
}

void DataPointConstraint::setAppliedForceAndVelocity(int index, double force,
    double velocity)
{
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

  copy->_source      = _source;
  copy->_destination = _destination;
  copy->_pose        = _pose;
  copy->_name        = _name;
  copy->_mode        = _mode;

  return copy;
}

/**
 * If used with MuscleActuator:
 * "source" should always be the segment. "destination" the sphere that connects
 * the segment to the MuscleActuator.
 */
void DataPointConstraint::createXsd(XsdSpecification& spec)
{
  auto constraintDefinition = new XsdSequence(YARS_STRING_POINT_CONSTRAINT_DEFINITION);
  constraintDefinition->add(NA(YARS_STRING_NAME, YARS_STRING_XSD_STRING,
        false));
  constraintDefinition->add(NA(YARS_STRING_MODE, YARS_STRING_XSD_STRING,
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
