#include <yars/configuration/data/DataGenericActuator.h>
#include <yars/configuration/data/DataDomainFactory.h>
#include <yars/configuration/data/DataPoseFactory.h>
#include <yars/configuration/data/DataPIDFactory.h>

#include <yars/defines/mutex.h>
#include <yars/types/Quaternion.h>
#include <yars/util/YarsErrorHandler.h>
#include <yars/util/noise/NoiseFactory.h>

# define YARS_STRING_ACTUATOR_AV_TYPE_DEFINITION (char*)"actuator_type_angular_velocity_force_definition"
# define YARS_STRING_ACTUATOR_PV_TYPE_DEFINITION (char*)"actuator_type_positional_velocity_force_definition"
# define YARS_STRING_ACTUATOR_MODE_DEFINITION (char*)"actuator_mode_active_passive_definition"

# define YARS_STRING_FRICTION                      (char*)"friction"
# define YARS_STRING_PRECISION                     (char*)"precision"
# define YARS_STRING_FORCE                         (char*)"force"
# define YARS_STRING_FORCE_DEFINITION              (char*)"actuator_force_definition"
# define YARS_STRING_FORCE_SCALING                 (char*)"scaling"
# define YARS_STRING_BIAS                          (char*)"bias"
# define YARS_STRING_TYPE                          (char*)"type"
# define YARS_STRING_MODE                          (char*)"mode"
# define YARS_STRING_SOURCE                        (char*)"source"
# define YARS_STRING_DESTINATION                   (char*)"destination"
# define YARS_STRING_MAPPING                       (char*)"mapping"
# define YARS_STRING_POSE                          (char*)"pose"
# define YARS_STRING_POSEG_DEFINITION              (char*)"pose_with_global_definition"
# define YARS_STRING_ROTATIONAL_AXIS_DEFINITION    (char*)"axis_rotational_definition"
# define YARS_STRING_TRANSLATIONAL_AXIS_DEFINITION (char*)"axis_translational_definition"
# define YARS_STRING_DEFLECTION                    (char*)"deflection"
# define YARS_STRING_ANGULAR                       (char*)"angular"
# define YARS_STRING_POSITIONAL                    (char*)"positional"
# define YARS_STRING_VELOCITY                      (char*)"velocity"
# define YARS_STRING_VELOCITY_DEFINITION           (char*)"actuator_velocity_definition"
# define YARS_STRING_ACTIVE                        (char*)"active"
# define YARS_STRING_MAXIMUM                       (char*)"max"
# define YARS_STRING_SCALING                       (char*)"scaling"
# define YARS_STRING_PASSIVE                       (char*)"passive"
# define YARS_STRING_NAME                          (char*)"name"
# define YARS_STRING_JOINT_AXIS_DEFINITION         (char*)"joint_axis_definition"
# define YARS_STRING_MIN_MAX_DEFINITION            (char*)"min_max_definition"
# define YARS_STRING_NAME_DEFINITION               (char*)"name_definition"
# define YARS_STRING_ACTUATOR_PARAMETER_DEFINITION (char*)"actuator_parameter_definition"
# define YARS_STRING_GLOBAL                        (char*)"global"
# define YARS_STRING_XYZG_DEFINITION               (char*)"xyzg_definition"
# define YARS_STRING_RAD_DEG_DEFINITION            (char*)"radOrDeg_definition"

# define YARS_STRING_ROTATIONAL                    (char*)"rotational"
# define YARS_STRING_TRANSLATIONAL                 (char*)"translational"

# define YARS_STRING_ROTATIONAL_DEFINITION         (char*)"rotational_definition"
# define YARS_STRING_TRANSLATIONAL_DEFINITION      (char*)"translational_definition"

# define YARS_STRING_X_AXIS                        (char*)"x"
# define YARS_STRING_Y_AXIS                        (char*)"y"
# define YARS_STRING_Z_AXIS                        (char*)"z"

# define YARS_STRING_REGULAR                       (char*)"regular"
# define YARS_STRING_LIMIT                         (char*)"limit"
# define YARS_STRING_ORTHOGONAL                    (char*)"orthogonal"

# define YARS_STRING_SOFTNESS                      (char*)"softness"
# define YARS_STRING_RESTITUTION                   (char*)"restitution"
# define YARS_STRING_DAMPING                       (char*)"damping"
# define YARS_STRING_STIFFNESS                     (char*)"stiffness"
# define YARS_STRING_SPRING                        (char*)"spring"
# define YARS_STRING_SPRING_DEFINITION             (char*)"spring_definition"

# define YARS_STRING_PID                           (char*)"pid"


# define X_ROT_AXIS (_parsingRotational    && _parsingXAxis)
# define Y_ROT_AXIS (_parsingRotational    && _parsingYAxis)
# define Z_ROT_AXIS (_parsingRotational    && _parsingZAxis)
# define X_TRA_AXIS (_parsingTranslational && _parsingXAxis)
# define Y_TRA_AXIS (_parsingTranslational && _parsingYAxis)
# define Z_TRA_AXIS (_parsingTranslational && _parsingZAxis)

# define DO_IN(p, v)\
       p.internalValue = p.internalDomain.cut(v);\
       p.externalValue = p.internalExternalMapping.map(p.internalValue);

# define DO_EX(p, v)\
      p.externalValue = p.externalDomain.cut(p.n->calculate(v));\
      p.internalValue = p.internalExternalMapping.invMap(p.externalValue);

# define DO_DE(p, v)\
       p.exDesiredValue = p.externalDomain.cut(v); \
       p.inDesiredValue = p.internalExternalMapping.invMap(p.exDesiredValue);

# define SCALE_DOMAIN(p)\
  if(p.controlType == DATA_ACTUATOR_CONTROL_ANGULAR)\
  {\
    p.deflection.min = DEG_TO_RAD(p.deflection.min);\
    p.deflection.max = DEG_TO_RAD(p.deflection.max);\
  }


DataGenericActuator::DataGenericActuator(DataNode *parent)
  : DataActuator(parent, DATA_ACTUATOR_GENERIC)
{
  _parsingRotational          = false;
  _parsingTranslational       = false;
  _parsingXAxis               = false;
  _parsingYAxis               = false;
  _parsingZAxis               = false;
  _useSpring                  = false;
  _poseInWorldCoordinates     = false;

  _mapping.resize(6);
  _angularVelocity.resize(3);
  _linearVelocity.resize(3);
  _angle.resize(3);
  _position.resize(3);
  _appliedForce.resize(6);
  _appliedVelocity.resize(6);

  for(int i = 0; i < 6; i++) _mapping[i]         = -1;
  for(int i = 0; i < 3; i++) _angularVelocity[i] = 0.0;
  for(int i = 0; i < 3; i++) _linearVelocity[i]  = 0.0;
  for(int i = 0; i < 3; i++) _angle[i]           = 0.0;
  for(int i = 0; i < 3; i++) _position[i]        = 0.0;
  for(int i = 0; i < 6; i++) _appliedForce[i]    = 0.0;
  for(int i = 0; i < 6; i++) _appliedVelocity[i] = 0.0;

  YM_INIT;
}

DataGenericActuator::~DataGenericActuator()
{
  YM_CLOSE;
}

string DataGenericActuator::name()
{
  return _name;
}

string DataGenericActuator::source()
{
  return _source;
}

string DataGenericActuator::destination()
{
  return _destination;
}

void DataGenericActuator::applyOffset(Pose offset)
{
  if(_poseInWorldCoordinates) return;
  _pose << offset;
  _axisPosition      = _pose.position;
  _axisOrientation   = _pose.orientation;
}

GenericParamter DataGenericActuator::parameter()
{
  YM_LOCK;
  GenericParamter r = _parameter;
  YM_UNLOCK;
  return r;
}


DataGenericActuator* DataGenericActuator::_copy()
{
  DataGenericActuator *copy = new DataGenericActuator(NULL);
  copy->_pose               = _pose;
  copy->_parameter          = _parameter;
  copy->_destination        = _destination;
  copy->_name               = _name;
  copy->_source             = _source;
  copy->_dimension          = _dimension;
  copy->_useSpring          = _useSpring;
  copy->_axisOrientation    = _axisOrientation;
  copy->_axisPosition       = _axisPosition;
  copy->__setMapping();
  return copy;
}

double DataGenericActuator::internalValue(int index)
{
  if(index < 0 || index >= _dimension) YarsErrorHandler::push("DataGenericActuator::internalValue error");

  YM_LOCK;
  double r = 0.0;
  switch(_mapping[index])
  {
    case 0: r = _parameter.rotational.x.internalValue;    break;
    case 1: r = _parameter.rotational.y.internalValue;    break;
    case 2: r = _parameter.rotational.z.internalValue;    break;
    case 3: r = _parameter.translational.x.internalValue; break;
    case 4: r = _parameter.translational.y.internalValue; break;
    case 5: r = _parameter.translational.z.internalValue; break;
  }
  YM_UNLOCK;
  return r;
}

double DataGenericActuator::externalValue(int index)
{
  if(index < 0 || index >= _dimension) YarsErrorHandler::push("DataGenericActuator::externalValue error");
  YM_LOCK;
  double r = 0.0;
  switch(_mapping[index])
  {
    case 0: r = _parameter.rotational.x.externalValue;    break;
    case 1: r = _parameter.rotational.y.externalValue;    break;
    case 2: r = _parameter.rotational.z.externalValue;    break;
    case 3: r = _parameter.translational.x.externalValue; break;
    case 4: r = _parameter.translational.y.externalValue; break;
    case 5: r = _parameter.translational.z.externalValue; break;
  }
  YM_UNLOCK;
  return r;
}

void DataGenericActuator::setInternalValue(int index, double v)
{
  if(index < 0 || index >= _dimension) YarsErrorHandler::push("DataGenericActuator::setInternalValue error");
  YM_LOCK;
  switch(_mapping[index])
  {
    case 0: DO_IN(_parameter.rotational.x,    v); break;
    case 1: DO_IN(_parameter.rotational.y,    v); break;
    case 2: DO_IN(_parameter.rotational.z,    v); break;
    case 3: DO_IN(_parameter.translational.x, v); break;
    case 4: DO_IN(_parameter.translational.y, v); break;
    case 5: DO_IN(_parameter.translational.z, v); break;
  }
  YM_UNLOCK;
}

void DataGenericActuator::setExternalValue(int index, double v)
{
  // cout << "setting " << index << " to " << v << endl;
  if(index < 0 || index >= _dimension) YarsErrorHandler::push("DataGenericActuator::setExternalValue error");
  YM_LOCK;
  switch(_mapping[index])
  {
    case 0: DO_EX(_parameter.rotational.x,    v); break;
    case 1: DO_EX(_parameter.rotational.y,    v); break;
    case 2: DO_EX(_parameter.rotational.z,    v); break;
    case 3: DO_EX(_parameter.translational.x, v); break;
    case 4: DO_EX(_parameter.translational.y, v); break;
    case 5: DO_EX(_parameter.translational.z, v); break;
  }
  YM_UNLOCK;
}

void DataGenericActuator::__setMapping()
{
  _dimension = 0;
  if(_parameter.rotational.x.isDefined)    _mapping[_dimension++] = 0;
  if(_parameter.rotational.y.isDefined)    _mapping[_dimension++] = 1;
  if(_parameter.rotational.z.isDefined)    _mapping[_dimension++] = 2;
  if(_parameter.translational.x.isDefined) _mapping[_dimension++] = 3;
  if(_parameter.translational.y.isDefined) _mapping[_dimension++] = 4;
  if(_parameter.translational.z.isDefined) _mapping[_dimension++] = 5;

  if(_parameter.rotational.x.isDefined)    __setDomains(_parameter.rotational.x);
  if(_parameter.rotational.y.isDefined)    __setDomains(_parameter.rotational.y);
  if(_parameter.rotational.z.isDefined)    __setDomains(_parameter.rotational.z);
  if(_parameter.translational.x.isDefined) __setDomains(_parameter.translational.x);
  if(_parameter.translational.y.isDefined) __setDomains(_parameter.translational.y);
  if(_parameter.translational.z.isDefined) __setDomains(_parameter.translational.z);
}

void DataGenericActuator::__setDomains(AxisParameter &p)
{
  p.externalDomain = p.mapping;
  switch(p.controlType)
  {
    case DATA_ACTUATOR_CONTROL_ANGULAR:
      p.internalDomain = p.deflection;
      break;
    case DATA_ACTUATOR_CONTROL_POSITIONAL:
      p.internalDomain = p.deflection;
      break;
    case DATA_ACTUATOR_CONTROL_VELOCITY:
      p.internalDomain.min = -p.velocity;
      p.internalDomain.max =  p.velocity;
      break;
    case DATA_ACTUATOR_CONTROL_FORCE:
      p.internalDomain.min = -1;
      p.internalDomain.max =  1;
      break;
  }
  p.internalExternalMapping.setInputDomain(p.internalDomain);
  p.internalExternalMapping.setOutputDomain(p.externalDomain);
  p.n = NoiseFactory::create(p.noise);
}


Domain DataGenericActuator::getInternalDomain(int index)
{
  if(index < 0 || index >= _dimension) YarsErrorHandler::push("DataGenericActuator::getInternalDomain error");
  YM_LOCK;
  Domain d;
  switch(_mapping[index])
  {
    case 0: d = _parameter.rotational.x.internalDomain;    break;
    case 1: d = _parameter.rotational.y.internalDomain;    break;
    case 2: d = _parameter.rotational.z.internalDomain;    break;
    case 3: d = _parameter.translational.x.internalDomain; break;
    case 4: d = _parameter.translational.y.internalDomain; break;
    case 5: d = _parameter.translational.z.internalDomain; break;
  }
  YM_UNLOCK;
  return d;
}

Domain DataGenericActuator::getExternalDomain(int index)
{
  if(index < 0 || index >= _dimension) YarsErrorHandler::push("DataGenericActuator::getExternalDomain error");
  YM_LOCK;
  Domain d;
  switch(_mapping[index])
  {
    case 0: d = _parameter.rotational.x.externalDomain;    break;
    case 1: d = _parameter.rotational.y.externalDomain;    break;
    case 2: d = _parameter.rotational.z.externalDomain;    break;
    case 3: d = _parameter.translational.x.externalDomain; break;
    case 4: d = _parameter.translational.y.externalDomain; break;
    case 5: d = _parameter.translational.z.externalDomain; break;
  }
  YM_UNLOCK;
  return d;
}

void DataGenericActuator::setDesiredValue(int index, double v)
{
  if(index < 0 || index >= _dimension) YarsErrorHandler::push("DataGenericActuator::setDesiredValue error");
  // cout << "DataGenericActuator: " << index << " " << v << endl;
  YM_LOCK;
  switch(_mapping[index])
  {
    case 0: DO_DE(_parameter.rotational.x,    v); break;
    case 1: DO_DE(_parameter.rotational.y,    v); break;
    case 2: DO_DE(_parameter.rotational.z,    v); break;
    case 3: DO_DE(_parameter.translational.x, v); break;
    case 4: DO_DE(_parameter.translational.y, v); break;
    case 5: DO_DE(_parameter.translational.z, v); break;
  }
  // cout << _parameter.rotational.x.exDesiredValue << " " << _parameter.rotational.y.exDesiredValue << " " << _parameter.rotational.z.exDesiredValue << endl;
  // cout << _parameter.rotational.x.inDesiredValue << " " << _parameter.rotational.y.inDesiredValue << " " << _parameter.rotational.z.inDesiredValue << endl;


  YM_UNLOCK;
}

double DataGenericActuator::getInternalDesiredValue(int index)
{
  if(index < 0 || index >= _dimension) YarsErrorHandler::push("DataGenericActuator::getInternalDesiredValue error");
  YM_LOCK;
  double r = 0.0;
  switch(_mapping[index])
  {
    case 0: r = _parameter.rotational.x.inDesiredValue;    break;
    case 1: r = _parameter.rotational.y.inDesiredValue;    break;
    case 2: r = _parameter.rotational.z.inDesiredValue;    break;
    case 3: r = _parameter.translational.x.inDesiredValue; break;
    case 4: r = _parameter.translational.y.inDesiredValue; break;
    case 5: r = _parameter.translational.z.inDesiredValue; break;
  }
  YM_UNLOCK;
  return r;
}

double DataGenericActuator::getExternalDesiredValue(int index)
{
  if(index < 0 || index >= _dimension) YarsErrorHandler::push("DataGenericActuator::getExternalDesiredValue error");
  YM_LOCK;
  double r = 0.0;
  switch(_mapping[index])
  {
    case 0: r = _parameter.rotational.x.exDesiredValue;    break;
    case 1: r = _parameter.rotational.y.exDesiredValue;    break;
    case 2: r = _parameter.rotational.z.exDesiredValue;    break;
    case 3: r = _parameter.translational.x.exDesiredValue; break;
    case 4: r = _parameter.translational.y.exDesiredValue; break;
    case 5: r = _parameter.translational.z.exDesiredValue; break;
  }
  YM_UNLOCK;
  return r;
}

Pose DataGenericActuator::pose()
{
  YM_LOCK;
  Pose p = _pose;
  YM_UNLOCK;
  return p;
}

void DataGenericActuator::createXsd(XsdSpecification *spec)
{

  // ROTATIONAL AXIS DEFINITION - BEGIN
  XsdSequence *rotAxisDefinition = new XsdSequence(YARS_STRING_ROTATIONAL_AXIS_DEFINITION);
  rotAxisDefinition->add(NA(YARS_STRING_TYPE,          YARS_STRING_ACTUATOR_AV_TYPE_DEFINITION,   true));
  rotAxisDefinition->add(NA(YARS_STRING_FRICTION,      YARS_STRING_POSITIVE_DECIMAL,              false));
  rotAxisDefinition->add(NA(YARS_STRING_MODE,          YARS_STRING_ACTUATOR_MODE_DEFINITION,      true));
  rotAxisDefinition->add(NE(YARS_STRING_FORCE,         YARS_STRING_FORCE_DEFINITION,              1, 1));
  rotAxisDefinition->add(NE(YARS_STRING_VELOCITY,      YARS_STRING_VELOCITY_DEFINITION,           1, 1));
  rotAxisDefinition->add(NE(YARS_STRING_DEFLECTION,    YARS_STRING_MIN_MAX_DEFINITION,            0, 1));
  rotAxisDefinition->add(NE(YARS_STRING_MAPPING,       YARS_STRING_MIN_MAX_DEFINITION,            0, 1));
  rotAxisDefinition->add(NE(YARS_STRING_LIMIT,         YARS_STRING_ACTUATOR_PARAMETER_DEFINITION, 0, 1));
  rotAxisDefinition->add(NE(YARS_STRING_SPRING,        YARS_STRING_SPRING_DEFINITION,             0, 1));
  rotAxisDefinition->add(NE(YARS_STRING_PID,           YARS_STRING_PID_DEFINITION,                0, 1));
  rotAxisDefinition->add(NE(YARS_STRING_NOISE,         YARS_STRING_NOISE_DEFINITION,              0, 1));
  rotAxisDefinition->add(NE(YARS_STRING_FILTER,        YARS_STRING_FILTER_DEFINITION,             0, 1));
  spec->add(rotAxisDefinition);
  // ROTATIONAL AXIS DEFINITION - END

  // TRANSLATIONAL AXIS DEFINITION - BEGIN
  XsdSequence *transAxisDefinition = new XsdSequence(YARS_STRING_TRANSLATIONAL_AXIS_DEFINITION);
  transAxisDefinition->add(NA(YARS_STRING_TYPE,       YARS_STRING_ACTUATOR_PV_TYPE_DEFINITION, true));
  transAxisDefinition->add(NA(YARS_STRING_FRICTION,   YARS_STRING_POSITIVE_DECIMAL,            false));
  transAxisDefinition->add(NA(YARS_STRING_MODE,       YARS_STRING_ACTUATOR_MODE_DEFINITION,    true));
  transAxisDefinition->add(NE(YARS_STRING_FORCE,      YARS_STRING_FORCE_DEFINITION,            1, 1));
  transAxisDefinition->add(NE(YARS_STRING_VELOCITY,   YARS_STRING_VELOCITY_DEFINITION,         1, 1));
  transAxisDefinition->add(NE(YARS_STRING_DEFLECTION, YARS_STRING_MIN_MAX_DEFINITION,          0, 1));
  transAxisDefinition->add(NE(YARS_STRING_MAPPING,    YARS_STRING_MIN_MAX_DEFINITION,          0, 1));
  transAxisDefinition->add(NE(YARS_STRING_SPRING,     YARS_STRING_SPRING_DEFINITION,           0, 1));
  transAxisDefinition->add(NE(YARS_STRING_PID,        YARS_STRING_PID_DEFINITION,              0, 1));
  transAxisDefinition->add(NE(YARS_STRING_NOISE,      YARS_STRING_NOISE_DEFINITION,            0, 1));
  transAxisDefinition->add(NE(YARS_STRING_FILTER,     YARS_STRING_FILTER_DEFINITION,           0, 1));
  spec->add(transAxisDefinition);
  // TRANSLATIONAL AXIS DEFINITION - END


  XsdSequence *genericDefinition = new XsdSequence(YARS_STRING_GENERIC_DEFINITION);
  genericDefinition->add(NA(YARS_STRING_NAME,          YARS_STRING_XSD_STRING,               true));
  genericDefinition->add(NA(YARS_STRING_SPRING,        YARS_STRING_TRUE_FALSE_DEFINITION,    false));
  genericDefinition->add(NE(YARS_STRING_SOURCE,        YARS_STRING_NAME_DEFINITION,          1, 1));
  genericDefinition->add(NE(YARS_STRING_DESTINATION,   YARS_STRING_NAME_DEFINITION,          0, 1));
  genericDefinition->add(NE(YARS_STRING_POSE,          YARS_STRING_POSEG_DEFINITION,         1, 1));
  genericDefinition->add(NE(YARS_STRING_ROTATIONAL,    YARS_STRING_ROTATIONAL_DEFINITION,    0, 1));
  genericDefinition->add(NE(YARS_STRING_TRANSLATIONAL, YARS_STRING_TRANSLATIONAL_DEFINITION, 0, 1));
  spec->add(genericDefinition);

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

  XsdSequence *rotAxesDefinition = new XsdSequence(YARS_STRING_ROTATIONAL_DEFINITION);
  rotAxesDefinition->add(NE(YARS_STRING_X_AXIS, YARS_STRING_ROTATIONAL_AXIS_DEFINITION, 0, 1));
  rotAxesDefinition->add(NE(YARS_STRING_Y_AXIS, YARS_STRING_ROTATIONAL_AXIS_DEFINITION, 0, 1));
  rotAxesDefinition->add(NE(YARS_STRING_Z_AXIS, YARS_STRING_ROTATIONAL_AXIS_DEFINITION, 0, 1));
  spec->add(rotAxesDefinition);

  XsdSequence *transAxesDefinition = new XsdSequence(YARS_STRING_TRANSLATIONAL_DEFINITION);
  transAxesDefinition->add(NE(YARS_STRING_LIMIT,  YARS_STRING_ACTUATOR_PARAMETER_DEFINITION, 0, 1));
  transAxesDefinition->add(NE(YARS_STRING_X_AXIS, YARS_STRING_TRANSLATIONAL_AXIS_DEFINITION, 0, 1));
  transAxesDefinition->add(NE(YARS_STRING_Y_AXIS, YARS_STRING_TRANSLATIONAL_AXIS_DEFINITION, 0, 1));
  transAxesDefinition->add(NE(YARS_STRING_Z_AXIS, YARS_STRING_TRANSLATIONAL_AXIS_DEFINITION, 0, 1));
  spec->add(transAxesDefinition);

  XsdSequence *springDefinition = new XsdSequence(YARS_STRING_SPRING_DEFINITION);
  springDefinition->add(NA(YARS_STRING_DAMPING,   YARS_STRING_POSITIVE_DECIMAL, true));
  springDefinition->add(NA(YARS_STRING_STIFFNESS, YARS_STRING_POSITIVE_DECIMAL, true));
  spec->add(springDefinition);

  XsdEnumeration *actuatorModeDefinition = new XsdEnumeration(YARS_STRING_ACTUATOR_MODE_DEFINITION,
      YARS_STRING_XSD_STRING);
  actuatorModeDefinition->add(YARS_STRING_ACTIVE);
  actuatorModeDefinition->add(YARS_STRING_PASSIVE);
  spec->add(actuatorModeDefinition);

  XsdSequence *forceParameter = new XsdSequence(YARS_STRING_FORCE_DEFINITION);
  forceParameter->add(NA(YARS_STRING_MAXIMUM, YARS_STRING_POSITIVE_DECIMAL, true));
  forceParameter->add(NA(YARS_STRING_SCALING, YARS_STRING_POSITIVE_DECIMAL, false));
  spec->add(forceParameter);

  XsdSequence *velocityParameter = new XsdSequence(YARS_STRING_VELOCITY_DEFINITION);
  velocityParameter->add(NA(YARS_STRING_MAXIMUM, YARS_STRING_POSITIVE_DECIMAL, true));
  spec->add(velocityParameter);
}

double DataGenericActuator::velocity(int index)
{
  YM_LOCK;
  double r = 0.0;
  switch(_mapping[index])
  {
    case 0: r = _parameter.rotational.x.velocity;    break;
    case 1: r = _parameter.rotational.y.velocity;    break;
    case 2: r = _parameter.rotational.z.velocity;    break;
    case 3: r = _parameter.translational.x.velocity; break;
    case 4: r = _parameter.translational.y.velocity; break;
    case 5: r = _parameter.translational.z.velocity; break;
  }
  YM_UNLOCK;
  return r;
}

double DataGenericActuator::force(int index)
{
  YM_LOCK;
  double r = 0.0;
  switch(_mapping[index])
  {
    case 0: r = _parameter.rotational.x.maxForce;    break;
    case 1: r = _parameter.rotational.y.maxForce;    break;
    case 2: r = _parameter.rotational.z.maxForce;    break;
    case 3: r = _parameter.translational.x.maxForce; break;
    case 4: r = _parameter.translational.y.maxForce; break;
    case 5: r = _parameter.translational.z.maxForce; break;
  }
  YM_UNLOCK;
  return r;
}

void DataGenericActuator::setVelocity(int index, double v)
{

}

void DataGenericActuator::setForce(int index, double f)
{

}

bool DataGenericActuator::isActive(int index)
{
  YM_LOCK;
  bool r = false;
  switch(_mapping[index])
  {
    case 0: r = _parameter.rotational.x.isActive;    break;
    case 1: r = _parameter.rotational.y.isActive;    break;
    case 2: r = _parameter.rotational.z.isActive;    break;
    case 3: r = _parameter.translational.x.isActive; break;
    case 4: r = _parameter.translational.y.isActive; break;
    case 5: r = _parameter.translational.z.isActive; break;
  }
  YM_UNLOCK;
  return r;
}

void DataGenericActuator::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_GENERIC))
  {
    current = parent;

    if(_parameter.rotational.x.deflection.min < -180 ||
       _parameter.rotational.x.deflection.max >  180 ||
       _parameter.rotational.y.deflection.min < -90  ||
       _parameter.rotational.y.deflection.max >  90  ||
       _parameter.rotational.x.deflection.min < -180 ||
       _parameter.rotational.x.deflection.max >  180)
    {
      stringstream oss;
      oss << "Generic Actuator limits may only be:" << endl;
      oss << "  X-Axis: [-180, 180]" << endl;
      oss << "  Y-Axis: [ -90,  90]" << endl;
      oss << "  Z-Axis: [-180, 180]" << endl;
      oss << "  Your configuration is: "   << endl;
      oss << "    X-Axis: " << _parameter.rotational.x.deflection << endl;
      oss << "    Y-Axis: " << _parameter.rotational.y.deflection << endl;
      oss << "    Z-Axis: " << _parameter.rotational.z.deflection << endl;
      YarsErrorHandler::push(oss.str());
    }

    SCALE_DOMAIN(_parameter.rotational.x);
    SCALE_DOMAIN(_parameter.rotational.y);
    SCALE_DOMAIN(_parameter.rotational.z);

    __setMapping();
  }
  if(element->opening(YARS_STRING_GENERIC))
  {
    element->set(YARS_STRING_NAME,   _name);
    element->set(YARS_STRING_SPRING, _useSpring);
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

  if(element->closing(YARS_STRING_ROTATIONAL) || element->closing(YARS_STRING_TRANSLATIONAL))
  {
    _parsingRotational    = false;
    _parsingTranslational = false;
  }

  if(element->opening(YARS_STRING_ROTATIONAL))
  {
    _parsingRotational    = true;
    _parsingTranslational = false;
  }

  if(element->opening(YARS_STRING_TRANSLATIONAL))
  {
    _parsingRotational    = false;
    _parsingTranslational = true;
  }

  if(element->closing(YARS_STRING_ROTATIONAL) ||
     element->closing(YARS_STRING_TRANSLATIONAL))
  {
    _parsingRotational    = false;
    _parsingTranslational = false;
  }

  if(element->closing(YARS_STRING_X_AXIS) ||
     element->closing(YARS_STRING_Y_AXIS) ||
     element->closing(YARS_STRING_Z_AXIS))
  {
    _parsingXAxis = false;
    _parsingYAxis = false;
    _parsingYAxis = false;
  }

  if(element->opening(YARS_STRING_X_AXIS))
  {
    _parsingXAxis = true;
    _parsingYAxis = false;
    _parsingZAxis = false;
    if(_parsingRotational)    __setParameters(_parameter.rotational.x,    element);
    if(_parsingTranslational) __setParameters(_parameter.translational.x, element);
  }
  if(element->opening(YARS_STRING_Y_AXIS))
  {
    _parsingXAxis = false;
    _parsingYAxis = true;
    _parsingZAxis = false;
    if(_parsingRotational)    __setParameters(_parameter.rotational.y,    element);
    if(_parsingTranslational) __setParameters(_parameter.translational.y, element);
  }
  if(element->opening(YARS_STRING_Z_AXIS))
  {
    _parsingXAxis = false;
    _parsingYAxis = false;
    _parsingZAxis = true;
    if(_parsingRotational)    __setParameters(_parameter.rotational.z,    element);
    if(_parsingTranslational) __setParameters(_parameter.translational.z, element);
  }

  if(element->opening(YARS_STRING_LIMIT))
  {
    if(X_ROT_AXIS) __setAxisParameters(_parameter.rotational.x.parameter, element);
    if(Y_ROT_AXIS) __setAxisParameters(_parameter.rotational.y.parameter, element);
    if(Z_ROT_AXIS) __setAxisParameters(_parameter.rotational.z.parameter, element);

    if(Z_TRA_AXIS) __setAxisParameters(_parameter.translational.z.parameter, element);
  }

  if(element->opening(YARS_STRING_SPRING))
  {
    if(X_ROT_AXIS) __setAxisSpringParameters(_parameter.rotational.x, element);
    if(Y_ROT_AXIS) __setAxisSpringParameters(_parameter.rotational.y, element);
    if(Z_ROT_AXIS) __setAxisSpringParameters(_parameter.rotational.z, element);

    if(X_TRA_AXIS) __setAxisSpringParameters(_parameter.translational.x, element);
    if(Y_TRA_AXIS) __setAxisSpringParameters(_parameter.translational.y, element);
    if(Z_TRA_AXIS) __setAxisSpringParameters(_parameter.translational.z, element);
  }

  if(element->opening(YARS_STRING_FORCE))
  {
    if(X_ROT_AXIS) __setAxisForceParameters(_parameter.rotational.x, element);
    if(Y_ROT_AXIS) __setAxisForceParameters(_parameter.rotational.y, element);
    if(Z_ROT_AXIS) __setAxisForceParameters(_parameter.rotational.z, element);
    if(X_TRA_AXIS) __setAxisForceParameters(_parameter.translational.x, element);
    if(Y_TRA_AXIS) __setAxisForceParameters(_parameter.translational.y, element);
    if(Z_TRA_AXIS) __setAxisForceParameters(_parameter.translational.z, element);
  }

  if(element->opening(YARS_STRING_VELOCITY))
  {
    if(X_ROT_AXIS) __setAxisVelocityParameters(_parameter.rotational.x, element);
    if(Y_ROT_AXIS) __setAxisVelocityParameters(_parameter.rotational.y, element);
    if(Z_ROT_AXIS) __setAxisVelocityParameters(_parameter.rotational.z, element);
    if(X_TRA_AXIS) __setAxisVelocityParameters(_parameter.translational.x, element);
    if(Y_TRA_AXIS) __setAxisVelocityParameters(_parameter.translational.y, element);
    if(Z_TRA_AXIS) __setAxisVelocityParameters(_parameter.translational.z, element);
  }

  if(element->opening(YARS_STRING_DEFLECTION))
  {
    if(X_ROT_AXIS) DataDomainFactory::set(_parameter.rotational.x.deflection,    element);
    if(Y_ROT_AXIS) DataDomainFactory::set(_parameter.rotational.y.deflection,    element);
    if(Z_ROT_AXIS) DataDomainFactory::set(_parameter.rotational.z.deflection,    element);
    if(X_TRA_AXIS) DataDomainFactory::set(_parameter.translational.x.deflection, element);
    if(Y_TRA_AXIS) DataDomainFactory::set(_parameter.translational.y.deflection, element);
    if(Z_TRA_AXIS) DataDomainFactory::set(_parameter.translational.z.deflection, element);

    if(X_ROT_AXIS) _parameter.rotational.x.isDeflectionSet    = true;
    if(Y_ROT_AXIS) _parameter.rotational.y.isDeflectionSet    = true;
    if(Z_ROT_AXIS) _parameter.rotational.z.isDeflectionSet    = true;
    if(X_TRA_AXIS) _parameter.translational.x.isDeflectionSet = true;
    if(Y_TRA_AXIS) _parameter.translational.y.isDeflectionSet = true;
    if(Z_TRA_AXIS) _parameter.translational.z.isDeflectionSet = true;
  }

  if(element->opening(YARS_STRING_MAPPING))
  {
    if(X_ROT_AXIS) DataDomainFactory::set(_parameter.rotational.x.mapping,    element);
    if(Y_ROT_AXIS) DataDomainFactory::set(_parameter.rotational.y.mapping,    element);
    if(Z_ROT_AXIS) DataDomainFactory::set(_parameter.rotational.z.mapping,    element);
    if(X_TRA_AXIS) DataDomainFactory::set(_parameter.translational.x.mapping, element);
    if(Y_TRA_AXIS) DataDomainFactory::set(_parameter.translational.y.mapping, element);
    if(Z_TRA_AXIS) DataDomainFactory::set(_parameter.translational.z.mapping, element);
  }

  if(element->opening(YARS_STRING_PID))
  {
    if(X_ROT_AXIS) DataPIDFactory::set(_parameter.rotational.x.pid,    element);
    if(Y_ROT_AXIS) DataPIDFactory::set(_parameter.rotational.y.pid,    element);
    if(Z_ROT_AXIS) DataPIDFactory::set(_parameter.rotational.z.pid,    element);
    if(X_TRA_AXIS) DataPIDFactory::set(_parameter.translational.x.pid, element);
    if(Y_TRA_AXIS) DataPIDFactory::set(_parameter.translational.y.pid, element);
    if(Z_TRA_AXIS) DataPIDFactory::set(_parameter.translational.z.pid, element);
  }

  if(element->opening(YARS_STRING_SPRING))
  {
    if(X_ROT_AXIS) __setSpringParameters(_parameter.rotational.x,    element);
    if(Y_ROT_AXIS) __setSpringParameters(_parameter.rotational.y,    element);
    if(Z_ROT_AXIS) __setSpringParameters(_parameter.rotational.z,    element);
    if(X_TRA_AXIS) __setSpringParameters(_parameter.translational.x, element);
    if(Y_TRA_AXIS) __setSpringParameters(_parameter.translational.y, element);
    if(Z_TRA_AXIS) __setSpringParameters(_parameter.translational.z, element);
  }
}

void DataGenericActuator::__setParameters(AxisParameter &p, DataParseElement *element)
{
  p.isDefined = true;
  element->set(YARS_STRING_TYPE,          p.type);
  element->set(YARS_STRING_MODE,          p.mode);
  element->set(YARS_STRING_FRICTION,      p.friction);
  // element->set(YARS_STRING_FORCE,         p.force);
  // element->set(YARS_STRING_FORCE_SCALING, p.forceScaling);
  // element->set(YARS_STRING_VELOCITY,      p.velocity);

  if(p.type == YARS_STRING_POSITIONAL) p.controlType = DATA_ACTUATOR_CONTROL_POSITIONAL;
  if(p.type == YARS_STRING_ANGULAR)    p.controlType = DATA_ACTUATOR_CONTROL_ANGULAR;
  if(p.type == YARS_STRING_VELOCITY)   p.controlType = DATA_ACTUATOR_CONTROL_VELOCITY;
  if(p.mode == "active")               p.isActive    = true;
}

int DataGenericActuator::dimension()
{
  return _dimension;
}

vector<int> DataGenericActuator::mapping()
{
  return _mapping;
}

void DataGenericActuator::setAngularVelocity(int index, double v)
{
  YM_LOCK;
  _angularVelocity[index] = v;
  YM_UNLOCK;
}

void DataGenericActuator::setLinearVelocity(int index, double v)
{
  YM_LOCK;
  _linearVelocity[index] = v;
  YM_UNLOCK;
}

double DataGenericActuator::getAngularVelocity(int index)
{
  YM_LOCK;
  double r = _angularVelocity[index];
  YM_UNLOCK;
  return r;
}

double DataGenericActuator::getLinearVelocity(int index)
{
  YM_LOCK;
  double r = _linearVelocity[index];
  YM_UNLOCK;
  return r;
}

void DataGenericActuator::setAngle(int i, double v)
{
  YM_LOCK;
  _angle[i] = v;
  YM_UNLOCK;
}

double DataGenericActuator::getAngle(int i)
{
  YM_LOCK;
  double r = _angle[i];
  YM_UNLOCK;
  return r;
}

void DataGenericActuator::setPosition(int i, double v)
{
  YM_LOCK;
  _position[i] = v;
  YM_UNLOCK;
}

double DataGenericActuator::getPosition(int i)
{
  YM_LOCK;
  double r = _position[i];
  YM_UNLOCK;
  return r;
}

double DataGenericActuator::springDamping(int index)
{
  YM_LOCK;
  double r = 0.0;
  switch(_mapping[index])
  {
    case 0: r = _parameter.rotational.x.springDamping;    break;
    case 1: r = _parameter.rotational.y.springDamping;    break;
    case 2: r = _parameter.rotational.z.springDamping;    break;
    case 3: r = _parameter.translational.x.springDamping; break;
    case 4: r = _parameter.translational.y.springDamping; break;
    case 5: r = _parameter.translational.z.springDamping; break;
    default:
            YarsErrorHandler::push("DataGenericActuator::springDamping unknown index %d -> %d given", index, _mapping[index]);

  }
  YM_UNLOCK;
  return r;
}

double DataGenericActuator::springStiffness(int index)
{
  YM_LOCK;
  double r = 0.0;
  switch(_mapping[index])
  {
    case 0: r = _parameter.rotational.x.springStiffness;    break;
    case 1: r = _parameter.rotational.y.springStiffness;    break;
    case 2: r = _parameter.rotational.z.springStiffness;    break;
    case 3: r = _parameter.translational.x.springStiffness; break;
    case 4: r = _parameter.translational.y.springStiffness; break;
    case 5: r = _parameter.translational.z.springStiffness; break;
    default:
            YarsErrorHandler::push("DataGenericActuator::springStiffness unknown index %d -> %d given", index, _mapping[index]);
  }
  YM_UNLOCK;
  return r;
}

// bool DataGenericActuator::useSpring(int index)
// {
  // YM_LOCK;
  // bool r = false;
  // switch(_mapping[index])
  // {
    // case 0: r = _parameter.rotational.x.useSpring;    break;
    // case 1: r = _parameter.rotational.y.useSpring;    break;
    // case 2: r = _parameter.rotational.z.useSpring;    break;
    // case 3: r = _parameter.translational.x.useSpring; break;
    // case 4: r = _parameter.translational.y.useSpring; break;
    // case 5: r = _parameter.translational.z.useSpring; break;
    // default:
            // YarsErrorHandler::push("DataGenericActuator::useSpring unknown index %d -> %d given", index, _mapping[index]);
  // }
  // YM_UNLOCK;
  // return r;
// }


void DataGenericActuator::__setSpringParameters(AxisParameter &p, DataParseElement *element)
{
  element->set(YARS_STRING_DAMPING,   p.springDamping);
  element->set(YARS_STRING_STIFFNESS, p.springStiffness);
  // p.useSpring   = true;
  // p.useSoftness = true;
  // p.useDamping  = true;
}

void DataGenericActuator::__setAxisParameters(TypeParameter &p, DataParseElement *element)
{
  if(element->hasAttriute(YARS_STRING_RESTITUTION))
  {
    p.useResitution = true;
    element->set(YARS_STRING_RESTITUTION, p.restitution);
  }
  if(element->hasAttriute(YARS_STRING_DAMPING))
  {
    p.useDamping    = true;
    element->set(YARS_STRING_DAMPING,     p.damping);
  }

  if(element->hasAttriute(YARS_STRING_SOFTNESS))
  {
    p.useSoftness   = true;
    element->set(YARS_STRING_SOFTNESS,    p.softness);
  }
}


void DataGenericActuator::__setAxisSpringParameters(AxisParameter &p, DataParseElement *element)
{
  element->set(YARS_STRING_STIFFNESS, p.springStiffness);
  element->set(YARS_STRING_DAMPING,   p.springDamping);
}

void DataGenericActuator::__setAxisForceParameters(AxisParameter &p, DataParseElement *element)
{
  element->set(YARS_STRING_MAXIMUM, p.maxForce);
  element->set(YARS_STRING_SCALING, p.forceScaling);
}

void DataGenericActuator::__setAxisVelocityParameters(AxisParameter &p, DataParseElement *element)
{
  element->set(YARS_STRING_MAXIMUM, p.velocity);
}

bool DataGenericActuator::usesSprings()
{
  YM_LOCK;
  bool r = _useSpring;
  YM_UNLOCK;
  return r;
}

int DataGenericActuator::mapping(int index)
{
  return _mapping[index];
}

double DataGenericActuator::getAppliedForce(int index)
{
  return _appliedForce[index];
}

double DataGenericActuator::getAppliedVelocity(int index)
{
  return _appliedVelocity[index];
}

void DataGenericActuator::setAppliedForceAndVelocity(int index, double force, double velocity)
{
  _appliedForce[index]    = force;
  _appliedVelocity[index] = velocity;
}
