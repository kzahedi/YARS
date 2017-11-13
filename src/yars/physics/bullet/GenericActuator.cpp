#include <yars/physics/bullet/GenericActuator.h>
#include <yars/util/macros.h>

#include <math.h>

#define POSITIONAL 123400
#define VELOCITY   123500
#define FORCE      123600
#define ANGULAR    123700

#define MAP(min, max, value) ((value > max)?max:(value<min)?min:value)
#define _SIGN(value)         ((value < 0)?-1:1)
#define SCALE(a)             MIN((3.0/180.0 * M_PI), fabs(a)) / (3.0/180.0 * M_PI)

# define SET_ROT_DEFLECTION(i,p)\
    if(p.isDeflectionSet)\
    {\
      angLowerLimit[i] = p.deflection.min - 0.1;\
      angUpperLimit[i] = p.deflection.max + 0.1;\
    }

# define SET_LIN_DEFLECTION(i,p)\
    if(p.isDeflectionSet)\
    {\
      linLowerLimit[i] = p.deflection.min;\
      linUpperLimit[i] = p.deflection.max;\
    }

# define SET_ROT_DAMPING(i,p)\
    if(p.parameter.useResitution) _genericConstraint->getRotationalLimitMotor(i)->m_bounce = p.parameter.restitution;\
    if(p.parameter.useSoftness)   _genericConstraint->getRotationalLimitMotor(i)->m_limitSoftness = p.parameter.softness;\
    if(p.parameter.useDamping)    _genericConstraint->getRotationalLimitMotor(i)->m_bounce = p.parameter.damping;

# define ENABLE_MOTOR(i,p)\
  _genericConstraint->getRotationalLimitMotor(i)->m_enableMotor = p.isActive;

GenericActuator::GenericActuator(DataGenericActuator *data, Robot *robot)
  : Actuator("GenericActuator", data->source(), data->destination(), robot)
{
  _data      = data;
  _parameter = data->parameter();
  _dimension = data->dimension();
  _mapping   = data->mapping();
  _isVisualised = Data::instance()->current()->screens()->visualiseJoints();

  _angle.resize(3);
  _position.resize(3);
  _lastAngle.resize(3);
  _lastPosition.resize(3);
  _rotType.resize(3);
  _traType.resize(3);

  for(int i = 0; i < 3; i++) _rotType[i]      = 0;
  for(int i = 0; i < 3; i++) _traType[i]      = 0;
  for(int i = 0; i < 3; i++) _angle[i]        = 0;
  for(int i = 0; i < 3; i++) _lastAngle[i]    = 0;
  for(int i = 0; i < 3; i++) _position[i]     = 0;
  for(int i = 0; i < 3; i++) _lastPosition[i] = 0;

  Pose pose  = _data->pose();
  P3D axis   = pose.orientation;
  P3D anchor = pose.position;

  if(_sourceObject != NULL && _destinationObject != NULL)
  {
    _source                  = _sourceObject->rigidBody();
    btRigidBody *destination = _destinationObject->rigidBody();

    ::Quaternion qa(axis);
    btQuaternion q(qa.x, qa.y, qa.z, qa.w);

    btTransform transformA(q, btVector3(anchor.x, anchor.y, anchor.z));

    btTransform transformB = destination->getWorldTransform().inverse() * transformA;
                transformA = _source->getWorldTransform().inverse()     * transformA;

    _genericConstraint  = new btGeneric6DofConstraint(*_source, *destination, transformA, transformB, true);
  }
  else
  {
    _source   = _sourceObject->rigidBody();

    ::Quaternion qa(axis);
    btQuaternion q(qa.x, qa.y, qa.z, qa.w);

    btTransform transform = btTransform::getIdentity();
    transform.setOrigin(btVector3(_sourceObject->data()->pose().position.x,
                                  _sourceObject->data()->pose().position.y,
                                  _sourceObject->data()->pose().position.z));

    transform.getBasis().setRotation(q);

    _genericConstraint = new btGeneric6DofConstraint(*_source, transform, true);
  }

  btVector3 angLowerLimit(0.0, 0.0, 0.0);
  btVector3 angUpperLimit(0.0, 0.0, 0.0);
  btVector3 linLowerLimit(0.0, 0.0, 0.0);
  btVector3 linUpperLimit(0.0, 0.0, 0.0);

  SET_ROT_DEFLECTION( 0, _parameter.rotational.x);
  SET_ROT_DEFLECTION( 1, _parameter.rotational.y);
  SET_ROT_DEFLECTION( 2, _parameter.rotational.z);

  SET_LIN_DEFLECTION( 0, _parameter.translational.x);
  SET_LIN_DEFLECTION( 1, _parameter.translational.y);
  SET_LIN_DEFLECTION( 2, _parameter.translational.z);

  SET_ROT_DAMPING(    0, _parameter.rotational.x);
  SET_ROT_DAMPING(    1, _parameter.rotational.y);
  SET_ROT_DAMPING(    2, _parameter.rotational.z);

  ENABLE_MOTOR(       0, _parameter.rotational.x);
  ENABLE_MOTOR(       1, _parameter.rotational.y);
  ENABLE_MOTOR(       2, _parameter.rotational.z);

  if(_parameter.translational.parameter.useDamping)
    _genericConstraint->getTranslationalLimitMotor()->m_damping =
      _parameter.translational.parameter.damping;

  if(_parameter.translational.parameter.useSoftness)
    _genericConstraint->getTranslationalLimitMotor()->m_limitSoftness =
      _parameter.translational.parameter.softness;

  if(_parameter.translational.parameter.useResitution)
    _genericConstraint->getTranslationalLimitMotor()->m_restitution =
      _parameter.translational.parameter.restitution;

  if(_parameter.rotational.x.type    == "velocity")   _rotType[0] = VELOCITY;
  if(_parameter.rotational.x.type    == "angular")    _rotType[0] = ANGULAR;
  if(_parameter.rotational.x.type    == "force")      _rotType[0] = FORCE;

  if(_parameter.rotational.y.type    == "velocity")   _rotType[1] = VELOCITY;
  if(_parameter.rotational.y.type    == "angular")    _rotType[1] = ANGULAR;
  if(_parameter.rotational.y.type    == "force")      _rotType[1] = FORCE;

  if(_parameter.rotational.z.type    == "velocity")   _rotType[2] = VELOCITY;
  if(_parameter.rotational.z.type    == "angular")    _rotType[2] = ANGULAR;
  if(_parameter.rotational.z.type    == "force")      _rotType[2] = FORCE;

  if(_parameter.translational.x.type == "velocity")   _traType[0] = VELOCITY;
  if(_parameter.translational.x.type == "positional") _traType[0] = POSITIONAL;
  if(_parameter.translational.x.type == "force")      _traType[0] = FORCE;

  if(_parameter.translational.y.type == "velocity")   _traType[1] = VELOCITY;
  if(_parameter.translational.y.type == "positional") _traType[1] = POSITIONAL;
  if(_parameter.translational.y.type == "force")      _traType[1] = FORCE;

  if(_parameter.translational.z.type == "velocity")   _traType[2] = VELOCITY;
  if(_parameter.translational.z.type == "positional") _traType[2] = POSITIONAL;
  if(_parameter.translational.z.type == "force")      _traType[2] = FORCE;

  _genericConstraint->getTranslationalLimitMotor()->m_enableMotor[0] = _parameter.translational.x.isActive;
  _genericConstraint->getTranslationalLimitMotor()->m_enableMotor[1] = _parameter.translational.y.isActive;
  _genericConstraint->getTranslationalLimitMotor()->m_enableMotor[2] = _parameter.translational.z.isActive;

  // for(int i = 0; i < 3; i++)
  // {
    // if(_genericConstraint->getTranslationalLimitMotor()->m_enableMotor[i] == false)
    // {
      // linLowerLimit[i] = 0.0;
      // linUpperLimit[i] = 0.0;
    // }
    // if(_genericConstraint->getRotationalLimitMotor(i)->m_enableMotor == false)
    // {
      // angLowerLimit[i] = 0.0;
      // angUpperLimit[i] = 0.0;
    // }
  // }

  // cout << linLowerLimit[0] << " " << linLowerLimit[1] << " " << linLowerLimit[2] << endl;
  // cout << linUpperLimit[0] << " " << linUpperLimit[1] << " " << linUpperLimit[2] << endl;
  // cout << angLowerLimit[0] << " " << angLowerLimit[1] << " " << angLowerLimit[2] << endl;
  // cout << angUpperLimit[0] << " " << angUpperLimit[1] << " " << angUpperLimit[2] << endl;

  _genericConstraint->setLinearLowerLimit(linLowerLimit);
  _genericConstraint->setLinearUpperLimit(linUpperLimit);
  _genericConstraint->setAngularLowerLimit(angLowerLimit);
  _genericConstraint->setAngularUpperLimit(angUpperLimit);

  for(int i = 0; i < 3; i++)
  {
    _genericConstraint->getTranslationalLimitMotor()->m_stopERP[i] = 1.0;
    _genericConstraint->getTranslationalLimitMotor()->m_stopCFM[i] = 0.0;
    _genericConstraint->getRotationalLimitMotor(i)->m_stopERP      = 1.0;
    _genericConstraint->getRotationalLimitMotor(i)->m_stopCFM      = 0.0;
  }

  for(int i = 0; i < 6; i++)
  {
    _genericConstraint->setParam(BT_CONSTRAINT_STOP_ERP, 1.0, i);
    _genericConstraint->setParam(BT_CONSTRAINT_STOP_CFM, 0.0, i);
  }

}

GenericActuator::~GenericActuator()
{
  if(_genericConstraint != NULL) delete _genericConstraint;
}


void GenericActuator::prePhysicsUpdate()
{
  for(int i = 0; i < _dimension; i++)
  {
    switch(_mapping[i])
    {
      case 0: __prePhysicsRot(0, i, _parameter.rotational.x);    break;
      case 1: __prePhysicsRot(1, i, _parameter.rotational.y);    break;
      case 2: __prePhysicsRot(2, i, _parameter.rotational.z);    break;
      case 3: __prePhysicsTra(0, i, _parameter.translational.x); break;
      case 4: __prePhysicsTra(1, i, _parameter.translational.y); break;
      case 5: __prePhysicsTra(2, i, _parameter.translational.z); break;
    }
  }
}

void GenericActuator::__prePhysicsRot(int axisIndex, int index, AxisParameter p)
{
  if(p.isActive)
  {
    switch(_rotType[axisIndex])
    {
      case ANGULAR:  __processAngular(axisIndex, index, p); break;
      case VELOCITY: __processRotVelocity(index, p); break;
      case FORCE:    __processRotForce(index, p); break;
    }
  }
  else
  {
    if(p.friction > 0.0)
    {
      _genericConstraint->getRotationalLimitMotor(axisIndex)->m_enableMotor    = true;
      _genericConstraint->getRotationalLimitMotor(axisIndex)->m_targetVelocity = 0.0;
      _genericConstraint->getRotationalLimitMotor(axisIndex)->m_maxMotorForce  = p.friction;
    }
  }
}

void GenericActuator::__prePhysicsTra(int axisIndex, int index, AxisParameter p)
{
  if(p.isActive)
  {
    switch(_traType[axisIndex])
    {
      case POSITIONAL: __processPositional(  axisIndex, index, p); break;
      case VELOCITY:   __processTraVelocity( axisIndex, index, p); break;
      case FORCE:      __processTraForce(    axisIndex, index, p); break;
    }
  }
  else
  {
    if(p.friction > 0.0)
    {
      _genericConstraint->getTranslationalLimitMotor()->m_enableMotor[axisIndex]    = true;
      _genericConstraint->getTranslationalLimitMotor()->m_maxMotorForce[axisIndex]  = p.friction;
      _genericConstraint->getTranslationalLimitMotor()->m_targetVelocity[axisIndex] = 0.0;
    }
  }
}

void GenericActuator::postPhysicsUpdate()
{
  _genericConstraint->calculateTransforms();
  for(int i = 0; i < 3; i++)
  {
    _angle[i] = _genericConstraint->getAngle(i);
    _data->setAngle(i, _angle[i]);
    _data->setAngularVelocity(i, (_angle[i] - _lastAngle[i]) * (double)__YARS_GET_SIMULATOR_FREQUENCY);
    _lastAngle[i] = _angle[i];
  }
  for(int i = 0; i < 3; i++)
  {
    _position[i] = _genericConstraint->getRelativePivotPosition(i);
    _data->setLinearVelocity(i, (_position[i] - _lastPosition[i]) * (double)__YARS_GET_SIMULATOR_FREQUENCY);
    _data->setPosition(i, _position[i]);
    _lastPosition[i] = _position[i];
  }
  if(_isVisualised)
  {
    btTransform  pose = _genericConstraint->getCalculatedTransformA();
    btVector3    vec  = pose.getOrigin();
    btQuaternion q    = pose.getRotation();
    _data->setCurrentAxisPosition(P3D(vec[0], vec[1], vec[2]));
    _data->setCurrentAxisOrientation(::Quaternion(q.getW(), q.getX(), q.getY(), q.getZ()));
  }
}

DataActuator* GenericActuator::data()
{
  return _data;
}

void GenericActuator::reset()
{
}

btTypedConstraint* GenericActuator::constraint()
{
  return _genericConstraint;
}


void GenericActuator::__processAngular(int axisIndex, int index, AxisParameter)
{
  double robotValue = _data->getInternalDesiredValue(index);

  // cout << robotValue << endl;

  // if(p.isDeflectionSet)
  // {
    // robotValue        = p.deflection.cut(robotValue);
    // _angle[axisIndex] = p.deflection.cut(_angle[axisIndex]);
  // }

  double diff       = robotValue - _angle[axisIndex];
  // double velocity   = p.pid.update(SIGN(diff) * _data->velocity(index));
  double velocity   = SCALE(diff) * _SIGN(diff) * _data->velocity(index);

  if(fabs(diff) < 0.01) velocity = 0.0;

  double force = _data->force(index);
  // cout << "Force: " << force << endl;
  // cout << _data->parameter();

  double fc = 0.0;
  switch(axisIndex)
  {
    case 0:
      fc = _parameter.rotational.x.forceScaling;
      break;
    case 1:
      fc = _parameter.rotational.y.forceScaling;
      break;
    case 2:
      fc = _parameter.rotational.z.forceScaling;
      break;
  }

  if(fc > 0.0)
  {
    force = force * MAX(0.01, fabs(tanh(fc * diff/M_PI)));
  }

  _genericConstraint->getRotationalLimitMotor(axisIndex)->m_targetVelocity = velocity;
  _genericConstraint->getRotationalLimitMotor(axisIndex)->m_maxMotorForce  = force;
}

void GenericActuator::__processRotVelocity(int index, AxisParameter p)
{
  double velocity   = p.pid.update(_data->getInternalDesiredValue(index));

  _genericConstraint->getRotationalLimitMotor(index)->m_targetVelocity = velocity;
  _genericConstraint->getRotationalLimitMotor(index)->m_maxMotorForce  = _data->force(index);
}

void GenericActuator::__processRotForce(int index, AxisParameter p)
{
  double v        = _data->getInternalDesiredValue(index);
  double force    = fabs(v) * _data->force(index);
  double velocity = p.pid.update(v  * _data->velocity(index));

  // cout << force << " " << velocity << endl;

  _genericConstraint->getRotationalLimitMotor(index)->m_targetVelocity = velocity;
  _genericConstraint->getRotationalLimitMotor(index)->m_maxMotorForce  = force;
}

void GenericActuator::__processPositional(int axisIndex, int index, AxisParameter p)
{
  double robotValue = _data->getInternalDesiredValue(index);

  if(p.isDeflectionSet)
  {
    robotValue           = p.deflection.cut(robotValue);
    _position[axisIndex] = p.deflection.cut(_position[axisIndex]);
  }
  double diff       = _position[axisIndex] - robotValue;
  double velocity   = p.pid.update(SIGN(diff) * _data->velocity(index));


  _genericConstraint->getTranslationalLimitMotor()->m_maxMotorForce[axisIndex]  = _data->force(index);
  _genericConstraint->getTranslationalLimitMotor()->m_targetVelocity[axisIndex] = velocity;
}

void GenericActuator::__processTraVelocity(int axisIndex, int index, AxisParameter p)
{
  double velocity;
  if(p.isActive)
    velocity = p.pid.update(_data->getInternalDesiredValue(index));
  else
    velocity = p.velocity;

  _genericConstraint->getTranslationalLimitMotor()->m_maxMotorForce[axisIndex] =
    _data->force(index);
  _genericConstraint->getTranslationalLimitMotor()->m_targetVelocity[axisIndex] =
    velocity;
}

void GenericActuator::__processTraForce(int axisIndex, int index, AxisParameter p)
{
  double v        = _data->getInternalDesiredValue(index);
  double force    = fabs(v) * _data->force(index);
  double velocity = p.pid.update(v  * _data->velocity(index));
  _genericConstraint->getTranslationalLimitMotor()->m_maxMotorForce[axisIndex]  = force;
  _genericConstraint->getTranslationalLimitMotor()->m_targetVelocity[axisIndex] = velocity;
}
