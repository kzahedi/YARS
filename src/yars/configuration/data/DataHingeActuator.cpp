#include <yars/configuration/data/DataHingeActuator.h>

#include <yars/configuration/data/DataDomainFactory.h>
#include <yars/configuration/data/DataPoseFactory.h>
#include <yars/configuration/data/DataPIDFactory.h>
#include <yars/util/macros.h>
#include <yars/types/TransformationMatrix.h>
#include <yars/types/Quaternion.h>
#include <yars/util/noise/NoiseFactory.h>
#include <yars/util/YarsErrorHandler.h>
#include <yars/defines/mutex.h>
#include <vector>

# define YARS_STRING_FRICTION                      (char*)"friction"
# define YARS_STRING_TYPE                          (char*)"type"
# define YARS_STRING_MODE                          (char*)"mode"
# define YARS_STRING_ACTUATOR_TYPE_DEFINITION      (char*)"actuator" DIVIDER "type" DIVIDER "angular" DIVIDER "velocity" DIVIDER "force" DIVIDER DEFINITION
# define YARS_STRING_ACTUATOR_MODE_DEFINITION      (char*)"actuator" DIVIDER "mode" DIVIDER "active" DIVIDER "passive" DIVIDER DEFINITION
# define YARS_STRING_SOURCE                        (char*)"source"
# define YARS_STRING_DESTINATION                   (char*)"destination"
# define YARS_STRING_MAPPING                       (char*)"mapping"
# define YARS_STRING_ORIENTATION                   (char*)"orientation"
# define YARS_STRING_ANCHOR                        (char*)"anchor"
# define YARS_STRING_PRECISION                     (char*)"precision"
# define YARS_STRING_DEFLECTION                    (char*)"deflection"
# define YARS_STRING_ANGULAR                       (char*)"angular"
# define YARS_STRING_VELOCITY                      (char*)"velocity"
# define YARS_STRING_VELOCITY_DEFINITION           (char*)"actuator_velocity" DIVIDER DEFINITION
# define YARS_STRING_FORCE                         (char*)"force"
# define YARS_STRING_FORCE_DEFINITION              (char*)"actuator_force" DIVIDER DEFINITION
# define YARS_STRING_MAXIMUM                       (char*)"max"
# define YARS_STRING_SCALING                       (char*)"scaling"
# define YARS_STRING_GLOBAL                        (char*)"global"
# define YARS_STRING_ACTIVE                        (char*)"active"
# define YARS_STRING_PASSIVE                       (char*)"passive"
# define YARS_STRING_NAME                          (char*)"name"
# define YARS_STRING_XYZ_DEFINITION                (char*)"xyz"       DIVIDER DEFINITION
# define YARS_STRING_XYZG_DEFINITION               (char*)"xyzg"      DIVIDER DEFINITION
# define YARS_STRING_MIN_MAX_DEFINITION            (char*)"min"       DIVIDER "max"      DIVIDER DEFINITION
# define YARS_STRING_NAME_DEFINITION               (char*)"name"      DIVIDER DEFINITION
# define YARS_STRING_ACTUATOR_PARAMETER_DEFINITION (char*) "actuator" DIVIDER "parameter" DIVIDER DEFINITION
# define YARS_STRING_RAD_DEG_DEFINITION            (char*)"radOrDeg"  DIVIDER DEFINITION

# define YARS_STRING_POSE             (char*)"pose"
# define YARS_STRING_POSEG_DEFINITION (char*)"pose_with_global" DIVIDER DEFINITION

# define YARS_STRING_REGULAR          (char*)"regular"
# define YARS_STRING_LIMIT            (char*)"limit"
# define YARS_STRING_ORTHOGONAL       (char*)"orthogonal"

# define YARS_STRING_SOFTNESS         (char*)"softness"
# define YARS_STRING_RESTITUTION      (char*)"restitution"
# define YARS_STRING_DAMPING          (char*)"damping"

# define YARS_STRING_PID              (char*)"pid"
# define YARS_STRING_UNIT_INTERVAL    (char*)"unit_interval"    DIVIDER DEFINITION

DataHingeActuator::DataHingeActuator(DataNode *parent)
  : DataActuator(parent, DATA_ACTUATOR_HINGE)
{
  _noise                  = new DataNoise(this);
  _filter                 = NULL;
  _deflectionSet          = false;
  _isActive               = true;
  _currentAngularVelocity = 0.0;
  _poseInWorldCoordinates = false;
  _appliedForce           = 0.0;
  _appliedVelocity        = 0.0;
  _friction               = 0.0;
  _n                      = NULL;

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
    _n      = NoiseFactory::create(_noise);
    current = parent;
  }

  if(element->opening(YARS_STRING_HINGE))
  {
    element->set(YARS_STRING_NAME,     _name);
    element->set(YARS_STRING_TYPE,     _jointType);
    element->set(YARS_STRING_FRICTION, _friction);
    element->set(YARS_STRING_MODE,     _mode);
    if(_mode == YARS_STRING_ACTIVE)  _isActive = true;
    if(_mode == YARS_STRING_PASSIVE) _isActive = false;
  }

  if(element->opening(YARS_STRING_FORCE))
  {
    element->set(YARS_STRING_MAXIMUM, _parameter.maxForce);
    element->set(YARS_STRING_SCALING, _parameter.forceScaling);
    // cout << "Hinge force " << _parameter.maxForce << " " << _parameter.forceScaling << endl;
  }

  if(element->opening(YARS_STRING_VELOCITY))
  {
    element->set(YARS_STRING_MAXIMUM, _parameter.maxVelocity);
    // cout << "Hinge velocity " << _parameter.maxVelocity << endl;
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

  _pose << offset;
  _axisPosition      = _pose.position;
  _axisOrientation   = _pose.orientation;
}

void DataHingeActuator::createXsd(XsdSpecification *spec)
{
  XsdSequence *hingeDefinition = new XsdSequence(YARS_STRING_HINGE_DEFINITION);
  hingeDefinition->add(NA(YARS_STRING_NAME,          YARS_STRING_XSD_STRING,               true));
  hingeDefinition->add(NA(YARS_STRING_TYPE,          YARS_STRING_ACTUATOR_TYPE_DEFINITION, true));
  hingeDefinition->add(NA(YARS_STRING_FRICTION,      YARS_STRING_POSITIVE_DECIMAL,         false));
  hingeDefinition->add(NA(YARS_STRING_MODE,          YARS_STRING_ACTUATOR_MODE_DEFINITION, true));
  hingeDefinition->add(NE(YARS_STRING_SOURCE,        YARS_STRING_NAME_DEFINITION,          1, 1));
  hingeDefinition->add(NE(YARS_STRING_DESTINATION,   YARS_STRING_NAME_DEFINITION,          0, 1));
  hingeDefinition->add(NE(YARS_STRING_FORCE,         YARS_STRING_FORCE_DEFINITION,         1, 1));
  hingeDefinition->add(NE(YARS_STRING_VELOCITY,      YARS_STRING_VELOCITY_DEFINITION,      1, 1));
  hingeDefinition->add(NE(YARS_STRING_POSE,          YARS_STRING_POSEG_DEFINITION,         1, 1));
  hingeDefinition->add(NE(YARS_STRING_DEFLECTION,    YARS_STRING_MIN_MAX_DEFINITION,       0, 1));
  hingeDefinition->add(NE(YARS_STRING_MAPPING,       YARS_STRING_MIN_MAX_DEFINITION,       0, 1));

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
  poseDefinition->add(NA(YARS_STRING_X,      YARS_STRING_XSD_DECIMAL,           false));
  poseDefinition->add(NA(YARS_STRING_Y,      YARS_STRING_XSD_DECIMAL,           false));
  poseDefinition->add(NA(YARS_STRING_Z,      YARS_STRING_XSD_DECIMAL,           false));
  poseDefinition->add(NA(YARS_STRING_ALPHA,  YARS_STRING_XSD_DECIMAL,           false));
  poseDefinition->add(NA(YARS_STRING_BETA,   YARS_STRING_XSD_DECIMAL,           false));
  poseDefinition->add(NA(YARS_STRING_GAMMA,  YARS_STRING_XSD_DECIMAL,           false));
  poseDefinition->add(NA(YARS_STRING_TYPE,   YARS_STRING_RAD_DEG_DEFINITION,    false));
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

  XsdSequence *forceParameter = new XsdSequence(YARS_STRING_FORCE_DEFINITION);
  forceParameter->add(NA(YARS_STRING_MAXIMUM, YARS_STRING_POSITIVE_DECIMAL, true));
  forceParameter->add(NA(YARS_STRING_SCALING, YARS_STRING_POSITIVE_DECIMAL, false));
  spec->add(forceParameter);

  XsdSequence *velocityParameter = new XsdSequence(YARS_STRING_VELOCITY_DEFINITION);
  velocityParameter->add(NA(YARS_STRING_MAXIMUM, YARS_STRING_POSITIVE_DECIMAL, true));
  spec->add(velocityParameter);
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
  YM_LOCK;
  bool r = _deflectionSet;
  YM_UNLOCK;
  return r;
}

HingeParameter DataHingeActuator::parameter()
{
  YM_LOCK;
  HingeParameter r = _parameter;
  YM_UNLOCK;
  return r;
}

string DataHingeActuator::mode()
{
  return _mode;
}

yReal DataHingeActuator::friction()
{
  return _friction;
}

DataHingeActuator* DataHingeActuator::_copy()
{
  DataHingeActuator *copy = new DataHingeActuator(NULL);

  if (_filter != NULL) copy->_filter = _filter->copy();
  if (_noise  != NULL) copy->_noise  = _noise->copy();
  copy->_deflection             = _deflection;
  copy->_mapping                = _mapping;
  copy->_parameter              = _parameter;
  copy->_pose                   = _pose;
  copy->_deflectionSet          = _deflectionSet;
  copy->_destination            = _destination;
  copy->_jointType              = _jointType;
  copy->_mode                   = _mode;
  copy->_name                   = _name;
  copy->_source                 = _source;
  copy->_isActive               = _isActive;
  copy->_controlType            = _controlType;
  copy->_axisOrientation        = _axisOrientation;
  copy->_axisPosition           = _axisPosition;
  copy->_friction               = _friction;
  copy->_poseInWorldCoordinates = _poseInWorldCoordinates;
  copy->__setMapping();

  return copy;
}

yReal DataHingeActuator::internalValue(int index)
{
  YM_LOCK;
  yReal r = _internalValue[index];
  YM_UNLOCK;
  return r;
}

yReal DataHingeActuator::externalValue(int index)
{
  YM_LOCK;
  yReal r = _externalValue[index];
  YM_UNLOCK;
  return r;
}

void DataHingeActuator::setInternalValue(int index, yReal v)
{
  YM_LOCK;
  _internalValue[index] = _internalDomain[index].cut(v);
  _externalValue[index] = _internalExternalMapping[index].map(_internalValue[index]);
  YM_UNLOCK;
}

void DataHingeActuator::setExternalValue(int index, yReal v)
{
  YM_LOCK;
  _externalValue[index] = _externalDomain[index].cut(_n->calculate(v));
  _internalValue[index] = _internalExternalMapping[index].invMap(_externalValue[index]);
  YM_UNLOCK;
}

void DataHingeActuator::__setMapping()
{
  if(_controlType == DATA_ACTUATOR_CONTROL_FORCE)
  {
    _internalValue.resize(2);
    _externalValue.resize(2);
    _desiredValue.resize(2);
    _desiredExValue.resize(2);
    _internalExternalMapping.resize(2);
    _internalDomain.resize(2);
    _externalDomain.resize(2);
  }
  else
  {
    _internalValue.resize(1);
    _externalValue.resize(1);
    _desiredValue.resize(1);
    _desiredExValue.resize(1);
    _internalExternalMapping.resize(1);
    _internalDomain.resize(1);
    _externalDomain.resize(1);
  }

  switch(_controlType)
  {
    case DATA_ACTUATOR_CONTROL_ANGULAR:
      _externalDomain[0] = _mapping;
      _internalDomain[0] = _deflection;
      _internalExternalMapping[0].setInputDomain(_internalDomain[0]);
      _internalExternalMapping[0].setOutputDomain(_externalDomain[0]);
      break;
    case DATA_ACTUATOR_CONTROL_VELOCITY:
      _externalDomain[0]     =  _mapping;
      _internalDomain[0].min = -_parameter.maxVelocity;
      _internalDomain[0].max =  _parameter.maxVelocity;
      _internalExternalMapping[0].setInputDomain(_internalDomain[0]);
      _internalExternalMapping[0].setOutputDomain(_externalDomain[0]);
      break;
    case DATA_ACTUATOR_CONTROL_FORCE:
      _internalDomain[0].min =   0.0;
      _internalDomain[0].max =  _parameter.maxForce;
      _externalDomain[0]     =  _mapping;
      _internalExternalMapping[0].setInputDomain(_internalDomain[0]);
      _internalExternalMapping[0].setOutputDomain(_externalDomain[0]);

      _internalDomain[1].min = -_parameter.maxVelocity;
      _internalDomain[1].max =  _parameter.maxVelocity;
      _externalDomain[1]     =  _mapping;
      // cout << "setting velocity to " << _internalDomain[1] << " " << _externalDomain[1] << endl;
      _internalExternalMapping[1].setInputDomain(_internalDomain[1]);
      _internalExternalMapping[1].setOutputDomain(_externalDomain[1]);
      break;
  }
  _n = NoiseFactory::create(_noise);
}

Domain DataHingeActuator::getInternalDomain(int index)
{
  YM_LOCK;
  Domain d = _internalDomain[index];
  YM_UNLOCK;
  return d;
}

Domain DataHingeActuator::getExternalDomain(int index)
{
  YM_LOCK;
  Domain d = _externalDomain[index];
  YM_UNLOCK;
  return d;
}

void DataHingeActuator::setDesiredValue(int index, yReal value)
{
  YM_LOCK;
  // cout << "DataHingeActuator::setDesiredValue: " << index << " = " << value;
  _desiredExValue[index] = _externalDomain[index].cut(value);
  // cout << " -> " << _desiredExValue[index];
  _desiredValue[index]   = _internalExternalMapping[index].invMap(_desiredExValue[index]);
  // cout << " -> " << _desiredValue[index] << endl;
  YM_UNLOCK;
}

yReal DataHingeActuator::getInternalDesiredValue(int index)
{
  YM_LOCK;
  yReal r = _desiredValue[index];
  YM_UNLOCK;
  return r;
}

yReal DataHingeActuator::getExternalDesiredValue(int index)
{
  YM_LOCK;
  yReal r = _desiredExValue[index];
  YM_UNLOCK;
  return r;
}

bool DataHingeActuator::isActive(int)
{
  YM_LOCK;
  bool r = _isActive;
  YM_UNLOCK;
  return r;
}

yReal DataHingeActuator::getCurrentAngularVelocity()
{
  YM_LOCK;
  yReal r = _currentAngularVelocity;
  YM_UNLOCK;
  return r;
}

void DataHingeActuator::setCurrentAngularVelocity(yReal v)
{
  YM_LOCK;
  _currentAngularVelocity = v;
  YM_UNLOCK;
}

void DataHingeActuator::setVelocity(yReal v)
{
  YM_LOCK;
  _parameter.maxVelocity = v;
  YM_UNLOCK;
}

void DataHingeActuator::setForce(yReal f)
{
  YM_LOCK;
  _parameter.maxForce = f;
  YM_UNLOCK;
}

yReal DataHingeActuator::velocity()
{
  YM_LOCK;
  yReal r = _parameter.maxVelocity;
  YM_UNLOCK;
  return r;
}

yReal DataHingeActuator::force()
{
  YM_LOCK;
  // cout << _name << " force: " << _parameter.maxForce << endl;
  yReal r = _parameter.maxForce;
  YM_UNLOCK;
  return r;
}

int DataHingeActuator::dimension()
{
  switch(_controlType)
  {
    case DATA_ACTUATOR_CONTROL_ANGULAR:
    case DATA_ACTUATOR_CONTROL_VELOCITY:
      return 1;
      break;
    case DATA_ACTUATOR_CONTROL_FORCE:
      return 2;
      break;
    default:
      YarsErrorHandler::push("Unknown type in DataHingeActuator::dimension()");
  }
  return -1;
}

yReal DataHingeActuator::getAppliedForce(int)
{
  return _appliedForce;
}

yReal DataHingeActuator::getAppliedVelocity(int)
{
  return _appliedVelocity;
}

void DataHingeActuator::setAppliedForceAndVelocity(int, yReal force, yReal velocity)
{
  _appliedForce    = force;
  _appliedVelocity = velocity;
}
