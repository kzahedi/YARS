#include "DataHingeActuator.h"
#include "DataDomainFactory.h"
#include "DataPoseFactory.h"
#include "DataPIDFactory.h"
#include "util/macros.h"
#include "types/TransformationMatrix.h"
#include "types/Quaternion.h"
#include "util/noise/NoiseFactory.h"
#include "defines/mutex.h"

# define YARS_STRING_TYPE                          (char*)"type"
# define YARS_STRING_MODE                          (char*)"mode"
# define YARS_STRING_ACTUATOR_TYPE_DEFINITION      (char*)"actuator_type_angular_velocity_force_definition"
# define YARS_STRING_ACTUATOR_MODE_DEFINITION      (char*)"actuator_mode_active_passive_definition"
# define YARS_STRING_SOURCE                        (char*)"source"
# define YARS_STRING_DESTINATION                   (char*)"destination"
# define YARS_STRING_MAPPING                       (char*)"mapping"
# define YARS_STRING_ORIENTATION                   (char*)"orientation"
# define YARS_STRING_ANCHOR                        (char*)"anchor"
# define YARS_STRING_PRECISION                     (char*)"precision"
# define YARS_STRING_DEFLECTION                    (char*)"deflection"
# define YARS_STRING_ANGULAR                       (char*)"angular"
# define YARS_STRING_VELOCITY                      (char*)"velocity"
# define YARS_STRING_FORCE                         (char*)"force"
# define YARS_STRING_GLOBAL                        (char*)"global"
# define YARS_STRING_ACTIVE                        (char*)"active"
# define YARS_STRING_PASSIVE                       (char*)"passive"
# define YARS_STRING_NAME                          (char*)"name"
# define YARS_STRING_XYZ_DEFINITION                (char*)"xyz_definition"
# define YARS_STRING_XYZG_DEFINITION               (char*)"xyzg_definition"
# define YARS_STRING_MIN_MAX_DEFINITION            (char*)"min_max_definition"
# define YARS_STRING_NAME_DEFINITION               (char*)"name_definition"
# define YARS_STRING_ACTUATOR_PARAMETER_DEFINITION (char*) "actuator_parameter_definition"
# define YARS_STRING_RAD_DEG_DEFINITION            (char*)"radOrDeg_definition"

# define YARS_STRING_POSE                          (char*)"pose"
# define YARS_STRING_POSEG_DEFINITION              (char*)"pose_with_global_definition"

# define YARS_STRING_REGULAR                       (char*)"regular"
# define YARS_STRING_LIMIT                         (char*)"limit"
# define YARS_STRING_ORTHOGONAL                    (char*)"orthogonal"

# define YARS_STRING_SOFTNESS                      (char*)"softness"
# define YARS_STRING_RESTITUTION                   (char*)"restitution"
# define YARS_STRING_DAMPING                       (char*)"damping"

# define YARS_STRING_PID                           (char*)"pid"


DataHingeActuator::DataHingeActuator(DataNode *parent)
  : DataActuator(parent, DATA_ACTUATOR_HINGE)
{
  _noise                      = NULL;
  _filter                     = NULL;
  _deflectionSet              = false;
  _desiredValue               = 0.0;
  _isActive                   = true;
  _currentAngularVelocity     = 0.0;
  _poseInWorldCoordinates     = false;

  YM_INIT;
}

DataHingeActuator::~DataHingeActuator()
{
  YM_CLOSE;
  delete _n;
}

void DataHingeActuator::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_HINGE))
  {
    __close();
    current = parent;
  }
  if(element->opening(YARS_STRING_HINGE))
  {
    element->set(YARS_STRING_NAME,     _name);
    element->set(YARS_STRING_TYPE,     _jointType);
    element->set(YARS_STRING_MODE,     _mode);
    element->set(YARS_STRING_FORCE,    _parameter.maxForce);
    element->set(YARS_STRING_VELOCITY, _parameter.maxVelocity);
    if(_mode == YARS_STRING_ACTIVE)  _isActive = true;
    if(_mode == YARS_STRING_PASSIVE) _isActive = false;
  }
  if(element->opening(YARS_STRING_SOURCE))
  {
    element->set(YARS_STRING_NAME, _source);
  }
  if(element->opening(YARS_STRING_DESTINATION))
  {
    element->set(YARS_STRING_NAME, _destination);
  }
  if(element->opening(YARS_STRING_POSE))
  {
    DataPoseFactory::set(_pose, element);
    element->set(YARS_STRING_GLOBAL, _poseInWorldCoordinates);
    _axisPosition    = _pose.position;
    _axisOrientation = _pose.orientation;
  }
  if(element->opening(YARS_STRING_DEFLECTION))
  {
    _deflectionSet = true;
    DataDomainFactory::set(_deflection, element);
  }
  if(element->opening(YARS_STRING_MAPPING))
  {
    DataDomainFactory::set(_mapping, element);
  }

  if(element->opening(YARS_STRING_REGULAR))
  {
    element->set(YARS_STRING_DAMPING,     _parameter.dampingDir);
    element->set(YARS_STRING_RESTITUTION, _parameter.restitutionDir);
    element->set(YARS_STRING_SOFTNESS,    _parameter.softnessDir);
  }
  if(element->opening(YARS_STRING_LIMIT))
  {
    element->set(YARS_STRING_DAMPING,     _parameter.dampingLim);
    element->set(YARS_STRING_RESTITUTION, _parameter.restitutionLim);
    element->set(YARS_STRING_SOFTNESS,    _parameter.softnessLim);
  }
  if(element->opening(YARS_STRING_ORTHOGONAL))
  {
    element->set(YARS_STRING_DAMPING,     _parameter.dampingOrtho);
    element->set(YARS_STRING_RESTITUTION, _parameter.restitutionOrtho);
    element->set(YARS_STRING_SOFTNESS,    _parameter.softnessOrtho);
  }
  if(element->opening(YARS_STRING_PID))
  {
    DataPIDFactory::set(_parameter.pid, element);
  }

  if(element->opening(YARS_STRING_NOISE))
  {
    _noise  = new DataNoise(this);
    current = _noise;
    _noise->add(element);
  }
  if(element->opening(YARS_STRING_FILTER))
  {
    _filter = new DataFilter(this);
    current = _filter;
    _filter->add(element);
  }
}

string DataHingeActuator::name()
{
  return _name;
}

string DataHingeActuator::source()
{
  return _source;
}

string DataHingeActuator::destination()
{
  return _destination;
}

Domain DataHingeActuator::deflection()
{
  return _deflection;
}

Pose   DataHingeActuator::pose()
{
  return _pose;
}

Domain DataHingeActuator::mapping()
{
  return _mapping;
}

DataNoise* DataHingeActuator::noise()
{
  return _noise;
}

DataFilter* DataHingeActuator::filter()
{
  return _filter;
}

string DataHingeActuator::jointType()
{
  return _jointType;
}

void DataHingeActuator::applyOffset(Pose offset)
{
  if(_poseInWorldCoordinates) return;

  Quaternion q(offset.orientation);
  _pose.position    += offset.position;
  _pose.orientation *= q;
  _axisPosition      = _pose.position;
  _axisOrientation   = _pose.orientation;
}

void DataHingeActuator::createXsd(XsdSpecification *spec)
{
  XsdSequence *hingeDefinition = new XsdSequence(YARS_STRING_HINGE_DEFINITION);
  hingeDefinition->add(NA(YARS_STRING_NAME,        YARS_STRING_XSD_STRING,               false));
  hingeDefinition->add(NA(YARS_STRING_TYPE,        YARS_STRING_ACTUATOR_TYPE_DEFINITION, true));
  hingeDefinition->add(NA(YARS_STRING_MODE,        YARS_STRING_ACTUATOR_MODE_DEFINITION, true));
  hingeDefinition->add(NA(YARS_STRING_FORCE,       YARS_STRING_POSITIVE_DECIMAL,         false));
  hingeDefinition->add(NA(YARS_STRING_VELOCITY,    YARS_STRING_POSITIVE_DECIMAL,         false));
  hingeDefinition->add(NE(YARS_STRING_SOURCE,      YARS_STRING_NAME_DEFINITION,          1, 1));
  hingeDefinition->add(NE(YARS_STRING_DESTINATION, YARS_STRING_NAME_DEFINITION,          0, 1));
  hingeDefinition->add(NE(YARS_STRING_POSE,        YARS_STRING_POSEG_DEFINITION,         1, 1));
  hingeDefinition->add(NE(YARS_STRING_DEFLECTION,  YARS_STRING_MIN_MAX_DEFINITION,       0, 1));
  hingeDefinition->add(NE(YARS_STRING_MAPPING,     YARS_STRING_MIN_MAX_DEFINITION,       0, 1));

  XsdElement *regularParameters = NE(YARS_STRING_REGULAR, "", 0, 1);
  regularParameters->add(NA(YARS_STRING_SOFTNESS,    YARS_STRING_POSITIVE_DECIMAL, false));
  regularParameters->add(NA(YARS_STRING_DAMPING,     YARS_STRING_POSITIVE_DECIMAL, false));
  regularParameters->add(NA(YARS_STRING_RESTITUTION, YARS_STRING_POSITIVE_DECIMAL, false));
  hingeDefinition->add(regularParameters);

  hingeDefinition->add(NE(YARS_STRING_LIMIT,       YARS_STRING_ACTUATOR_PARAMETER_DEFINITION, 0, 1));
  hingeDefinition->add(NE(YARS_STRING_ORTHOGONAL,  YARS_STRING_ACTUATOR_PARAMETER_DEFINITION, 0, 1));
  hingeDefinition->add(NE(YARS_STRING_PID,         YARS_STRING_PID_DEFINITION,                0, 1));
  hingeDefinition->add(NE(YARS_STRING_NOISE,       YARS_STRING_NOISE_DEFINITION,              0, 1));
  hingeDefinition->add(NE(YARS_STRING_FILTER,      YARS_STRING_FILTER_DEFINITION,             0, 1));
  spec->add(hingeDefinition);

  XsdEnumeration *actuatorTypeDefinition = new XsdEnumeration(YARS_STRING_ACTUATOR_TYPE_DEFINITION,
      YARS_STRING_XSD_STRING);
  actuatorTypeDefinition->add(YARS_STRING_VELOCITY);
  actuatorTypeDefinition->add(YARS_STRING_ANGULAR);
  actuatorTypeDefinition->add(YARS_STRING_FORCE);
  spec->add(actuatorTypeDefinition);

  XsdSequence *poseDefinition = new XsdSequence(YARS_STRING_POSEG_DEFINITION);
  poseDefinition->add(NA(YARS_STRING_X,     YARS_STRING_XSD_DECIMAL,        false));
  poseDefinition->add(NA(YARS_STRING_Y,     YARS_STRING_XSD_DECIMAL,        false));
  poseDefinition->add(NA(YARS_STRING_Z,     YARS_STRING_XSD_DECIMAL,        false));
  poseDefinition->add(NA(YARS_STRING_ALPHA, YARS_STRING_XSD_DECIMAL,        false));
  poseDefinition->add(NA(YARS_STRING_BETA,  YARS_STRING_XSD_DECIMAL,        false));
  poseDefinition->add(NA(YARS_STRING_GAMMA, YARS_STRING_XSD_DECIMAL,        false));
  poseDefinition->add(NA(YARS_STRING_TYPE,  YARS_STRING_RAD_DEG_DEFINITION, false));
  poseDefinition->add(NA(YARS_STRING_GLOBAL, YARS_STRING_TRUE_FALSE_DEFINITION, false));
  spec->add(poseDefinition);

  XsdEnumeration *actuatorModeDefinition = new XsdEnumeration(YARS_STRING_ACTUATOR_MODE_DEFINITION,
      YARS_STRING_XSD_STRING);
  actuatorModeDefinition->add(YARS_STRING_ACTIVE);
  actuatorModeDefinition->add(YARS_STRING_PASSIVE);
  spec->add(actuatorModeDefinition);

  XsdSequence *actuatorParameter = new XsdSequence(YARS_STRING_ACTUATOR_PARAMETER_DEFINITION);
  actuatorParameter->add(NA(YARS_STRING_SOFTNESS,    YARS_STRING_POSITIVE_DECIMAL, false));
  actuatorParameter->add(NA(YARS_STRING_DAMPING,     YARS_STRING_POSITIVE_DECIMAL, false));
  actuatorParameter->add(NA(YARS_STRING_RESTITUTION, YARS_STRING_POSITIVE_DECIMAL, false));
  spec->add(actuatorParameter);
}

void DataHingeActuator::__close()
{
  if(_jointType == YARS_STRING_ANGULAR)  _controlType = DATA_ACTUATOR_CONTROL_ANGULAR;
  if(_jointType == YARS_STRING_VELOCITY) _controlType = DATA_ACTUATOR_CONTROL_VELOCITY;
  if(_jointType == YARS_STRING_FORCE)    _controlType = DATA_ACTUATOR_CONTROL_FORCE;

  _deflection.min = DEG_TO_RAD(_deflection.min);
  _deflection.max = DEG_TO_RAD(_deflection.max);

  __setMapping();
}

bool DataHingeActuator::isDeflectionSet()
{
  return _deflectionSet;
}

HingeParameter DataHingeActuator::parameter()
{
  return _parameter;
}

string DataHingeActuator::mode()
{
  return _mode;
}

DataHingeActuator* DataHingeActuator::_copy()
{
  DataHingeActuator *copy = new DataHingeActuator(NULL);

  if (_filter != NULL) copy->_filter = _filter->copy();
  if (_noise  != NULL) copy->_noise  = _noise->copy();
  copy->_deflection         = _deflection;
  copy->_mapping            = _mapping;
  copy->_parameter          = _parameter;
  copy->_pose               = _pose;
  copy->_deflectionSet      = _deflectionSet;
  copy->_destination        = _destination;
  copy->_jointType          = _jointType;
  copy->_mode               = _mode;
  copy->_name               = _name;
  copy->_source             = _source;
  copy->_isActive           = _isActive;
  copy->_controlType        = _controlType;
  copy->_axisOrientation    = _axisOrientation;
  copy->_axisPosition       = _axisPosition;
  copy->__setMapping();

  return copy;
}

double DataHingeActuator::internalValue(int index)
{
  YM_LOCK;
  double r = _internalValue;
  YM_UNLOCK;
  return r;
}

double DataHingeActuator::externalValue(int index)
{
  YM_LOCK;
  double r = _externalValue;
  YM_UNLOCK;
  return r;
}

void DataHingeActuator::setInternalValue(int index, double v)
{
  YM_LOCK;
  _internalValue = _internalDomain.cut(v);
  _externalValue = _internalExternalMapping.map(_internalValue);
  YM_UNLOCK;
}

void DataHingeActuator::setExternalValue(int index, double v)
{
  YM_LOCK;
  _externalValue = _externalDomain.cut(_n->calculate(v));
  _internalValue = _internalExternalMapping.invMap(_externalValue);
  YM_UNLOCK;
}

void DataHingeActuator::__setMapping()
{
  _externalDomain = _mapping;
  switch(_controlType)
  {
    case DATA_ACTUATOR_CONTROL_ANGULAR:
      _internalDomain = _deflection;
      break;
    case DATA_ACTUATOR_CONTROL_VELOCITY:
      _internalDomain.min = -_parameter.maxVelocity;
      _internalDomain.max =  _parameter.maxVelocity;
      break;
    case DATA_ACTUATOR_CONTROL_FORCE:
      _internalDomain.min = -1;
      _internalDomain.max =  1;
      break;
  }
  _internalExternalMapping.setInputDomain(_internalDomain);
  _internalExternalMapping.setOutputDomain(_externalDomain);
  _n = NoiseFactory::create(_noise);
}

Domain DataHingeActuator::getInternalDomain(int index)
{
  YM_LOCK;
  Domaind r = _internalDomain;
  YM_UNLOCK;
  return r;
}

Domain DataHingeActuator::getExternalDomain(int index)
{
  YM_LOCK;
  Domaind r = _externalDomain;
  YM_UNLOCK;
  return r;
}

void DataHingeActuator::setDesiredValue(int index, double value)
{
  YM_LOCK;
  _desiredExValue = _externalDomain.cut(value);
  _desiredValue = _internalExternalMapping.invMap(_desiredExValue);
  YM_UNLOCK;
}

double DataHingeActuator::getInternalDesiredValue(int index)
{
  YM_LOCK;
  r = _desiredValue;
  YM_UNLOCK;
  return r;
}

double DataHingeActuator::getExternalDesiredValue(int index)
{
  YM_LOCK;
  r = _desiredExValue;
  YM_UNLOCK;
  return r;
}

bool DataHingeActuator::isActive(int index)
{
  return _isActive;
}

double DataHingeActuator::getCurrentAngularVelocity()
{
  YM_LOCK;
  r = _currentAngularVelocity;
  YM_UNLOCK;
  return r;
}

void DataHingeActuator::setCurrentAngularVelocity(double v)
{
  YM_LOCK;
  _currentAngularVelocity = v;
  YM_UNLOCK;
}

void DataHingeActuator::setVelocity(double v)
{
  YM_LOCK;
  _parameter.maxVelocity = v;
  YM_UNLOCK;
}

void DataHingeActuator::setForce(double f)
{
  YM_LOCK;
  _parameter.maxForce = f;
  YM_UNLOCK;
}

double DataHingeActuator::velocity()
{
  YM_LOCK;
  r = _parameter.maxVelocity;
  YM_UNLOCK;
  return r;
}

double DataHingeActuator::force()
{
  YM_LOCK;
  r = _parameter.maxForce;
  YM_UNLOCK;
  return r;
}
