#include <yars/configuration/data/DataSliderActuator.h>
#include "DataBinding.h"
#include <yars/configuration/data/DataDomainFactory.h>
#include <yars/configuration/data/DataPoseFactory.h>
#include <yars/configuration/data/DataPIDFactory.h>
#include <yars/util/macros.h>
#include <yars/types/TransformationMatrix.h>
#include <yars/types/Quaternion.h>
#include <yars/util/noise/NoiseFactory.h>
#include <yars/defines/mutex.h>

#include <cstdlib>

# define YARS_STRING_FRICTION                      (char*)"friction"
# define YARS_STRING_TYPE                          (char*)"type"
# define YARS_STRING_MODE                          (char*)"mode"
# define YARS_STRING_ACTUATOR_TYPE_DEFINITION      (char*)"actuator_type_positional_velocity_force_definition"
# define YARS_STRING_ACTUATOR_MODE_DEFINITION      (char*)"actuator_mode_active_passive_definition"
# define YARS_STRING_SOURCE                        (char*)"source"
# define YARS_STRING_DESTINATION                   (char*)"destination"
# define YARS_STRING_MAPPING                       (char*)"mapping"
# define YARS_STRING_POSE                          (char*)"pose"
# define YARS_STRING_POSEG_DEFINITION              (char*)"pose_with_global_definition"
# define YARS_STRING_ANCHOR                        (char*)"anchor"
# define YARS_STRING_PRECISION                     (char*)"precision"
# define YARS_STRING_DEFLECTION                    (char*)"deflection"
# define YARS_STRING_POSITIONAL                    (char*)"positional"
# define YARS_STRING_VELOCITY                      (char*)"velocity"
# define YARS_STRING_VELOCITY_DEFINITION           (char*)"slider_velocity_definition"
# define YARS_STRING_FORCE                         (char*)"force"
# define YARS_STRING_FORCE_DEFINITION              (char*)"force_definition"
# define YARS_STRING_FORCE_VELOCITY                (char*)"force and velocity"
# define YARS_STRING_MAXIMUM                       (char*)"max"
# define YARS_STRING_SCALING                       (char*)"scaling"
# define YARS_STRING_ACTIVE                        (char*)"active"
# define YARS_STRING_PASSIVE                       (char*)"passive"
# define YARS_STRING_NAME                          (char*)"name"
# define YARS_STRING_XYZ_DEFINITION                (char*)"xyz_definition"
# define YARS_STRING_MIN_MAX_DEFINITION            (char*)"min_max_definition"
# define YARS_STRING_NAME_DEFINITION               (char*)"name_definition"
# define YARS_STRING_ACTUATOR_PARAMETER_DEFINITION (char*)"actuator_parameter_definition"
# define YARS_STRING_RAD_DEG_DEFINITION            (char*)"radOrDeg_definition"

# define YARS_STRING_REGULAR                       (char*)"regular"
# define YARS_STRING_LIMIT                         (char*)"limit"
# define YARS_STRING_ORTHOGONAL                    (char*)"orthogonal"

# define YARS_STRING_SOFTNESS                      (char*)"softness"
# define YARS_STRING_RESTITUTION                   (char*)"restitution"
# define YARS_STRING_DAMPING                       (char*)"damping"

# define YARS_STRING_PID                           (char*)"pid"
# define YARS_STRING_GLOBAL                        (char*)"global"


namespace
{
// Attribute binding table for the slider's own opening tag. Child-element
// dispatch stays hand-written below.
const std::vector<yars::AttributeBinding> &sliderActuatorAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_NAME,
       [](DataNode *self, const std::string &value)
       { static_cast<DataSliderActuator *>(self)->setName(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_TYPE,
       [](DataNode *self, const std::string &value)
       { static_cast<DataSliderActuator *>(self)->setJointType(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_MODE,
       [](DataNode *self, const std::string &value)
       { static_cast<DataSliderActuator *>(self)->setMode(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_FRICTION,
       [](DataNode *self, const std::string &value)
       { static_cast<DataSliderActuator *>(self)->setFriction(atof(value.c_str())); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

DataSliderActuator::DataSliderActuator(DataNode *parent)
  : DataActuator(parent, DATA_ACTUATOR_SLIDER)
{
  _noise                       = std::make_unique<DataNoise>(this);
  _filter                      = NULL;
  _deflectionSet               = false;
  _isActive                    = true;
  _currentTransitionalVelocity = 0.0;
  _poseInWorldCoordinates      = false;
  _appliedForce                = 0.0;
  _appliedVelocity             = 0.0;
  _friction                    = 0.0;
  _n                           = NULL;

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

DataSliderActuator::~DataSliderActuator()
{
  YM_CLOSE;
  delete _n;
}

void DataSliderActuator::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_SLIDER))
  {
    __close();
    current = parent;
  }
  if(element->opening(YARS_STRING_SLIDER))
  {
    yars::applyAttributes(this, element, sliderActuatorAttributeBindings());
    if(_mode == YARS_STRING_ACTIVE)    _isActive = true;
    if(_mode == YARS_STRING_PASSIVE)   _isActive = false;
  }

  if(element->opening(YARS_STRING_FORCE))
  {
    element->set(YARS_STRING_MAXIMUM, _parameter.maxForce);
    element->set(YARS_STRING_SCALING, _parameter.forceScaling);
  }

  if(element->opening(YARS_STRING_VELOCITY))
  {
    element->set(YARS_STRING_MAXIMUM, _parameter.maxVelocity);
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
    _noise  = std::make_unique<DataNoise>(this);
    current = _noise.get();
    _noise->add(element);
  }
  if(element->opening(YARS_STRING_FILTER))
  {
    _filter = new DataFilter(this);
    current = _filter;
    _filter->add(element);
  }
}

string DataSliderActuator::name()
{
  return _name;
}

double DataSliderActuator::friction()
{
  return _friction;
}

string DataSliderActuator::source()
{
  return _source;
}

string DataSliderActuator::destination()
{
  return _destination;
}

Domain DataSliderActuator::deflection()
{
  return _deflection;
}

Pose DataSliderActuator::pose()
{
  YM_LOCK;
  Pose r = _pose;
  YM_UNLOCK;
  return r;
}

Domain DataSliderActuator::mapping()
{
  YM_LOCK;
  Domain r = _mapping;
  YM_UNLOCK;
  return r;
}

DataNoise* DataSliderActuator::noise()
{
  return _noise.get();
}

DataFilter* DataSliderActuator::filter()
{
  return _filter;
}

string DataSliderActuator::jointType()
{
  return _jointType;
}

void DataSliderActuator::applyOffset(Pose offset)
{
  if(_poseInWorldCoordinates) return;
  _pose << offset;
  _axisPosition      = _pose.position;
  _axisOrientation   = _pose.orientation;
}


void DataSliderActuator::__close()
{
  if(_jointType == YARS_STRING_POSITIONAL)     _controlType = DATA_ACTUATOR_CONTROL_POSITIONAL;
  if(_jointType == YARS_STRING_VELOCITY)       _controlType = DATA_ACTUATOR_CONTROL_VELOCITY;
  if(_jointType == YARS_STRING_FORCE)          _controlType = DATA_ACTUATOR_CONTROL_FORCE;
  if(_jointType == YARS_STRING_FORCE_VELOCITY) _controlType = DATA_ACTUATOR_CONTROL_FORCE_VELOCITY;

  __setMapping();
}

bool DataSliderActuator::isDeflectionSet()
{
  return _deflectionSet;
}

SliderParameter DataSliderActuator::parameter()
{
  YM_LOCK;
  SliderParameter r = _parameter;
  YM_UNLOCK;
  return r;
}

string DataSliderActuator::mode()
{
  return _mode;
}

void DataSliderActuator::setName(string name)
{
  _name = name;
}

void DataSliderActuator::setJointType(string jointType)
{
  _jointType = jointType;
}

void DataSliderActuator::setMode(string mode)
{
  _mode = mode;
}

void DataSliderActuator::setFriction(double friction)
{
  _friction = friction;
}

DataSliderActuator* DataSliderActuator::_copy()
{
  DataSliderActuator *copy = new DataSliderActuator(NULL);

  if (_filter != NULL) copy->_filter = _filter->copy();
  if (_noise  != nullptr) copy->_noise.reset(_noise->copy());
  copy->_mapping            = _mapping;
  copy->_parameter          = _parameter;
  copy->_pose               = _pose;
  copy->_deflection         = _deflection;
  copy->_deflectionSet      = _deflectionSet;
  copy->_destination        = _destination;
  copy->_jointType          = _jointType;
  copy->_mode               = _mode;
  copy->_name               = _name;
  copy->_source             = _source;
  copy->_friction           = _friction;
  copy->_isActive           = _isActive;
  copy->_controlType        = _controlType;
  copy->_axisOrientation    = _axisOrientation;
  copy->_axisPosition       = _axisPosition;
  copy->__setMapping();
  return copy;
}

double DataSliderActuator::velocity()
{
  YM_LOCK;
  double r = _parameter.maxVelocity;
  YM_UNLOCK;
  return r;
}

double DataSliderActuator::force()
{
  YM_LOCK;
  double r = _parameter.maxForce;
  YM_UNLOCK;
  return r;
}

double DataSliderActuator::internalValue(int index)
{
  YM_LOCK;
  double r = _internalValue[index];
  YM_UNLOCK;
  return r;
}

double DataSliderActuator::externalValue(int index)
{
  YM_LOCK;
  double r = _externalValue[index];
  YM_UNLOCK;
  return r;
}

void DataSliderActuator::setInternalValue(int index, double v)
{
  YM_LOCK;
  _internalValue[index] = _internalDomain[index].cut(v);
  _externalValue[index] = _internalExternalMapping[index].map(_internalValue[index]);
  YM_UNLOCK;
}

void DataSliderActuator::setExternalValue(int index, double v)
{
  YM_LOCK;
  _externalValue[index] = _externalDomain[index].cut(_n->calculate(v));
  _internalValue[index] = _internalExternalMapping[index].invMap(_externalValue[index]);
  YM_UNLOCK;
}

void DataSliderActuator::__setMapping()
{
  if(_controlType == DATA_ACTUATOR_CONTROL_FORCE_VELOCITY)
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
    case DATA_ACTUATOR_CONTROL_POSITIONAL:
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
      _externalDomain[0]     =  _mapping;
      _internalDomain[0].min = -_parameter.maxForce;
      _internalDomain[0].max =  _parameter.maxForce;
      _internalExternalMapping[0].setInputDomain(_internalDomain[0]);
      _internalExternalMapping[0].setOutputDomain(_externalDomain[0]);
      break;
    case DATA_ACTUATOR_CONTROL_FORCE_VELOCITY:
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
  _n = NoiseFactory::create(_noise.get());
}
Domain DataSliderActuator::getInternalDomain(int index)
{
  YM_LOCK;
  Domain r = _internalDomain[index];
  YM_UNLOCK;
  return r;
}

Domain DataSliderActuator::getExternalDomain(int index)
{
  YM_LOCK;
  Domain r = _externalDomain[index];
  YM_UNLOCK;
  return r;
}

void DataSliderActuator::setDesiredValue(int index, double value)
{
  YM_LOCK;
  _desiredExValue[index] = _externalDomain[index].cut(value);
  _desiredValue[index] = _internalExternalMapping[index].invMap(_desiredExValue[index]);
  YM_UNLOCK;
}

double DataSliderActuator::getInternalDesiredValue(int index)
{
  YM_LOCK;
  double r = _desiredValue[index];
  YM_UNLOCK;
  return r;
}

void DataSliderActuator::setVelocity(double v)
{
  YM_LOCK;
  _parameter.maxVelocity = v;
  YM_UNLOCK;
}

void DataSliderActuator::setForce(double f)
{
  YM_LOCK;
  _parameter.maxForce = f;
  YM_UNLOCK;
}

double DataSliderActuator::getExternalDesiredValue(int index)
{
  YM_LOCK;
  double r = _desiredExValue[index];
  YM_UNLOCK;
  return r;
}

bool DataSliderActuator::isActive(int index)
{
  return _isActive;
}

double DataSliderActuator::getCurrentTransitionalVelocity()
{
  YM_LOCK;
  double r = _currentTransitionalVelocity;
  YM_UNLOCK;
  return r;
}

void DataSliderActuator::setCurrentTransitionalVelocity(double v)
{
  YM_LOCK;
  _currentTransitionalVelocity = v;
  YM_UNLOCK;
}

void DataSliderActuator::setPosition(P3D position)
{
  YM_LOCK;
  _pose.position = position;
  YM_UNLOCK;
}

double DataSliderActuator::getAppliedForce(int index)
{
  return _appliedForce;
}

double DataSliderActuator::getAppliedVelocity(int index)
{
  return _appliedVelocity;
}

void DataSliderActuator::setAppliedForceAndVelocity(int index, double force, double velocity)
{
  _appliedForce    = force;
  _appliedVelocity = velocity;
}

