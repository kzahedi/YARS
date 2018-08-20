#include <yars/configuration/data/DataMuscleActuator.h>
#include <yars/configuration/data/DataDomainFactory.h>
#include <yars/configuration/data/DataPoseFactory.h>
#include <yars/configuration/data/DataPIDFactory.h>
#include <yars/configuration/data/DataObjectFactory.h>
#include <yars/util/macros.h>
#include <yars/types/TransformationMatrix.h>
#include <yars/types/Quaternion.h>
#include <yars/util/noise/NoiseFactory.h>
#include <yars/defines/mutex.h>

# define YARS_STRING_VISUALISE                     (char*)"visualise"
# define YARS_STRING_FRICTION                      (char*)"friction"
# define YARS_STRING_TYPE                          (char*)"type"
# define YARS_STRING_USE                           (char*)"use"
# define YARS_STRING_ACTUATOR_MODE_DEFINITION      (char*)"actuator_mode_active_passive_definition"
# define YARS_STRING_SOURCE                        (char*)"source"
# define YARS_STRING_DESTINATION                   (char*)"destination"
# define YARS_STRING_SOURCE_ANCHOR                 (char*)"srcAnchor"
# define YARS_STRING_DESTINATION_ANCHOR            (char*)"dstAnchor"
# define YARS_STRING_ANCHOR_DEFINITION             (char*)"anchor_definition"
# define YARS_STRING_MAPPING                       (char*)"mapping"
# define YARS_STRING_POSE                          (char*)"pose"
# define YARS_STRING_ANCHOR                        (char*)"anchor"
# define YARS_STRING_PRECISION                     (char*)"precision"
# define YARS_STRING_DEFLECTION                    (char*)"deflection"
# define YARS_STRING_POSITIONAL                    (char*)"positional"
# define YARS_STRING_VELOCITY                      (char*)"velocity"
# define YARS_STRING_VELOCITY_DEFINITION           (char*)"muscle_velocity_definition"
# define YARS_STRING_MUSFIB                        (char*)"musfib"
# define YARS_STRING_MUSFIB_DEFINITION             (char*)"muscle_musfib_definition"
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

# define YARS_STRING_C                             (char*)"c"
# define YARS_STRING_W                             (char*)"w"
# define YARS_STRING_LENGTH_COMPONENT_DEFINITION   (char*)"length_component_definition"
# define YARS_STRING_LENGTH_COMPONENT              (char*)"lengthComponent"
# define YARS_STRING_VELOCITY_COMPONENT_DEFINITION (char*)"velocity_component_definition"
# define YARS_STRING_VELOCITY_COMPONENT            (char*)"velocityComponent"
# define YARS_STRING_OPTIMAL_LENGTH                (char*)"optimalLength"
# define YARS_STRING_MAX_VELOCITY                  (char*)"maxVelocity"
# define YARS_STRING_N                             (char*)"N"
# define YARS_STRING_K                             (char*)"K"

DataMuscleActuator::DataMuscleActuator(DataNode *parent)
  : DataActuator(parent, DATA_ACTUATOR_MUSCLE)
{
  _noise                       = new DataNoise(this);
  _filter                      = NULL;
  _deflectionSet               = false;
  _currentTransitionalVelocity = 0.0;
  _appliedForce                = 0.0;
  _appliedVelocity             = 0.0;
  _n                           = NULL;
  _parsingSourceAnchor         = false;
  _parsingDestinationAnchor    = false;

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

DataMuscleActuator::~DataMuscleActuator()
{
  YM_CLOSE;
  delete _n;
}

void DataMuscleActuator::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_MUSCLE))
  {
    __close();
    current = parent;
  }
  if(element->opening(YARS_STRING_MUSCLE))
  {
    element->set(YARS_STRING_NAME,      _name);
    element->set(YARS_STRING_VISUALISE, _visualise);
  }

  if(element->opening(YARS_STRING_FORCE))
  {
    element->set(YARS_STRING_MAXIMUM, _parameter.maxForce);
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


  if(_parsingSourceAnchor)
  {
    _srcObject = DataObjectFactory::object(element, this);
    current = _srcObject;
    _parsingSourceAnchor = false;
  }
  if(element->opening(YARS_STRING_SOURCE_ANCHOR))
  {
    _parsingSourceAnchor = true;
  }

  if(_parsingDestinationAnchor == true)
  {
    _dstObject = DataObjectFactory::object(element, this);
    current = _dstObject;
    _parsingDestinationAnchor = false;
  }

  if(element->opening(YARS_STRING_DESTINATION_ANCHOR))
  {
    _parsingDestinationAnchor = true;
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

string DataMuscleActuator::name()
{
  return _name;
}

string DataMuscleActuator::source()
{
  return _source;
}

string DataMuscleActuator::destination()
{
  return _destination;
}

Domain DataMuscleActuator::deflection()
{
  return _deflection;
}

Domain DataMuscleActuator::mapping()
{
  YM_LOCK;
  Domain r = _mapping;
  YM_UNLOCK;
  return r;
}

DataNoise* DataMuscleActuator::noise()
{
  return _noise;
}

DataFilter* DataMuscleActuator::filter()
{
  return _filter;
}

void DataMuscleActuator::applyOffset(Pose offset)
{
  if(_srcOffsetInWorldCoordinates == false) _srcOffset << offset;
  if(_dstOffsetInWorldCoordinates == false) _dstOffset << offset;
}


void DataMuscleActuator::createXsd(XsdSpecification *spec)
{
  XsdSequence *muscleDefinition = new XsdSequence(YARS_STRING_MUSCLE_DEFINITION);
  muscleDefinition->add(NA(YARS_STRING_NAME,               YARS_STRING_XSD_STRING,                    true));
  muscleDefinition->add(NA(YARS_STRING_VISUALISE,          YARS_STRING_TRUE_FALSE_DEFINITION,         false));
  muscleDefinition->add(NE(YARS_STRING_SOURCE,             YARS_STRING_NAME_DEFINITION,               1, 1));
  muscleDefinition->add(NE(YARS_STRING_DESTINATION,        YARS_STRING_NAME_DEFINITION,               0, 1));
  muscleDefinition->add(NE(YARS_STRING_SOURCE_ANCHOR,      YARS_STRING_ANCHOR_DEFINITION,             1, 1));
  muscleDefinition->add(NE(YARS_STRING_DESTINATION_ANCHOR, YARS_STRING_ANCHOR_DEFINITION,             1, 1));
  muscleDefinition->add(NE(YARS_STRING_FORCE,              YARS_STRING_FORCE_DEFINITION,              1, 1));
  muscleDefinition->add(NE(YARS_STRING_VELOCITY,           YARS_STRING_VELOCITY_DEFINITION,           1, 1));
  muscleDefinition->add(NE(YARS_STRING_DEFLECTION,         YARS_STRING_MIN_MAX_DEFINITION,            0, 1));
  muscleDefinition->add(NE(YARS_STRING_MAPPING,            YARS_STRING_MIN_MAX_DEFINITION,            0, 1));
  muscleDefinition->add(NE(YARS_STRING_NOISE,              YARS_STRING_NOISE_DEFINITION,              0, 1));
  muscleDefinition->add(NE(YARS_STRING_FILTER,             YARS_STRING_FILTER_DEFINITION,             0, 1));
  muscleDefinition->add(NE(YARS_STRING_LENGTH_COMPONENT,   YARS_STRING_LENGTH_COMPONENT_DEFINITION,   0, 1));
  muscleDefinition->add(NE(YARS_STRING_VELOCITY_COMPONENT, YARS_STRING_VELOCITY_COMPONENT_DEFINITION, 0, 1));
  spec->add(muscleDefinition);

  XsdSequence *anchorDefinition = new XsdSequence(YARS_STRING_ANCHOR_DEFINITION);
  XsdChoice *objectChoice = new XsdChoice("",              "1", YARS_STRING_XSD_UNBOUNDED);
  objectChoice->add(NE(YARS_STRING_OBJECT_BOX,             YARS_STRING_OBJECT_BOX_DEFINTION,             0));
  objectChoice->add(NE(YARS_STRING_OBJECT_SPHERE,          YARS_STRING_OBJECT_SPHERE_DEFINTION,          0));
  objectChoice->add(NE(YARS_STRING_OBJECT_PLY,             YARS_STRING_OBJECT_PLY_DEFINTION,             0));
  objectChoice->add(NE(YARS_STRING_OBJECT_CYLINDER,        YARS_STRING_OBJECT_CYLINDER_DEFINTION,        0));
  objectChoice->add(NE(YARS_STRING_OBJECT_CAPPED_CYLINDER, YARS_STRING_OBJECT_CAPPED_CYLINDER_DEFINTION, 0));
  anchorDefinition->add(objectChoice);

  XsdElement *forceParameter = NE(YARS_STRING_FORCE_DEFINITION, "", 0, 1);
  forceParameter->add(NA(YARS_STRING_MAXIMUM, YARS_STRING_POSITIVE_DECIMAL, true));
  forceParameter->add(NA(YARS_STRING_SCALING, YARS_STRING_UNIT_INTERVAL,    false));
  muscleDefinition->add(forceParameter);

  XsdElement *velocityParameter = NE(YARS_STRING_VELOCITY_DEFINITION, "", 0, 1);
  velocityParameter->add(NA(YARS_STRING_MAXIMUM, YARS_STRING_POSITIVE_DECIMAL, true));
  muscleDefinition->add(velocityParameter);


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

  XsdSequence *lengthComponent = new XsdSequence(YARS_STRING_LENGTH_COMPONENT_DEFINITION);
  lengthComponent->add(NA(YARS_STRING_OPTIMAL_LENGTH, YARS_STRING_XSD_DECIMAL, true));
  lengthComponent->add(NA(YARS_STRING_W,              YARS_STRING_XSD_DECIMAL, true));
  lengthComponent->add(NA(YARS_STRING_C,              YARS_STRING_XSD_DECIMAL, true));
  lengthComponent->add(NA(YARS_STRING_USE,            YARS_STRING_TRUE_FALSE_DEFINITION, true));
  spec->add(lengthComponent);

  XsdSequence *velocityComponent = new XsdSequence(YARS_STRING_VELOCITY_COMPONENT_DEFINITION);
  velocityComponent->add(NA(YARS_STRING_MAX_VELOCITY, YARS_STRING_XSD_DECIMAL,           true));
  velocityComponent->add(NA(YARS_STRING_N,            YARS_STRING_XSD_DECIMAL,           true));
  velocityComponent->add(NA(YARS_STRING_K,            YARS_STRING_XSD_DECIMAL,           true));
  velocityComponent->add(NA(YARS_STRING_USE,          YARS_STRING_TRUE_FALSE_DEFINITION, true));
  spec->add(velocityComponent);

}

void DataMuscleActuator::__close()
{
  __setMapping();

  // 2. create ball joints
  // 3. create cylinders
  // create slider joint is done in MuscleActuator

}

bool DataMuscleActuator::isDeflectionSet()
{
  return _deflectionSet;
}

MuscleParameter DataMuscleActuator::parameter()
{
  YM_LOCK;
  MuscleParameter r = _parameter;
  YM_UNLOCK;
  return r;
}

string DataMuscleActuator::mode()
{
  return string("active");
}

DataMuscleActuator* DataMuscleActuator::_copy()
{
  DataMuscleActuator *copy = new DataMuscleActuator(NULL);

  if (_filter != NULL) copy->_filter = _filter->copy();
  if (_noise  != NULL) copy->_noise  = _noise->copy();
  copy->_mapping         = _mapping;
  copy->_parameter       = _parameter;
  copy->_deflection      = _deflection;
  copy->_deflectionSet   = _deflectionSet;
  copy->_destination     = _destination;
  copy->_name            = _name;
  copy->_source          = _source;
  copy->_axisOrientation = _axisOrientation;
  copy->_axisPosition    = _axisPosition;
  copy->_srcObject       = _srcObject->copy();
  copy->_dstObject       = _dstObject->copy();
  copy->__setMapping();
  return copy;
}

double DataMuscleActuator::velocity()
{
  YM_LOCK;
  double r = _parameter.maxVelocity;
  YM_UNLOCK;
  return r;
}

double DataMuscleActuator::force()
{
  YM_LOCK;
  double r = _parameter.maxForce;
  YM_UNLOCK;
  return r;
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

void DataMuscleActuator::setInternalValue(int index, double v)
{
  YM_LOCK;
  _internalValue[index] = _internalDomain[index].cut(v);
  _externalValue[index] = _internalExternalMapping[index].map(_internalValue[index]);
  YM_UNLOCK;
}

void DataMuscleActuator::setExternalValue(int index, double v)
{
  YM_LOCK;
  _externalValue[index] = _externalDomain[index].cut(_n->calculate(v));
  _internalValue[index] = _internalExternalMapping[index].invMap(_externalValue[index]);
  YM_UNLOCK;
}

void DataMuscleActuator::__setMapping()
{
  _externalDomain[0]     =  _mapping;
  _internalDomain[0].min = -_parameter.maxForce;
  _internalDomain[0].max =  _parameter.maxForce;
  _internalExternalMapping[0].setInputDomain(_internalDomain[0]);
  _internalExternalMapping[0].setOutputDomain(_externalDomain[0]);
  _n = NoiseFactory::create(_noise);
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

void DataMuscleActuator::setDesiredValue(int index, double value)
{
  YM_LOCK;
  _desiredExValue[index] = _externalDomain[index].cut(value);
  _desiredValue[index] = _internalExternalMapping[index].invMap(_desiredExValue[index]);
  YM_UNLOCK;
}

double DataMuscleActuator::getInternalDesiredValue(int index)
{
  YM_LOCK;
  double r = _desiredValue[index];
  YM_UNLOCK;
  return r;
}

void DataMuscleActuator::setVelocity(double v)
{
  YM_LOCK;
  _parameter.maxVelocity = v;
  YM_UNLOCK;
}

void DataMuscleActuator::setForce(double f)
{
  YM_LOCK;
  _parameter.maxForce = f;
  YM_UNLOCK;
}

double DataMuscleActuator::getExternalDesiredValue(int index)
{
  YM_LOCK;
  double r = _desiredExValue[index];
  YM_UNLOCK;
  return r;
}

bool DataMuscleActuator::isActive(int index)
{
  return true;
}

double DataMuscleActuator::getCurrentTransitionalVelocity()
{
  YM_LOCK;
  double r = _currentTransitionalVelocity;
  YM_UNLOCK;
  return r;
}

void DataMuscleActuator::setCurrentTransitionalVelocity(double v)
{
  YM_LOCK;
  _currentTransitionalVelocity = v;
  YM_UNLOCK;
}

void DataMuscleActuator::setPosition(P3D position)
{
}

double DataMuscleActuator::getAppliedForce(int index)
{
  return _appliedForce;
}

double DataMuscleActuator::getAppliedVelocity(int index)
{
  return _appliedVelocity;
}

void DataMuscleActuator::setAppliedForceAndVelocity(int index, double force, double velocity)
{
  _appliedForce    = force;
  _appliedVelocity = velocity;
}

Pose DataMuscleActuator::pose()
{
  Pose r;
  return r;
}

DataObject* DataMuscleActuator::sourceAnchor()
{
  return _srcObject;
}

DataObject* DataMuscleActuator::destinationAnchor()
{
  return _dstObject;
}

DataActuator* DataMuscleActuator::sourceConnector()
{
  return _srcConnector;
}

DataActuator* DataMuscleActuator::destinationConnector()
{
  return _dstConnector;
}
