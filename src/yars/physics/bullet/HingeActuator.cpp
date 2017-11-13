#include <yars/physics/bullet/HingeActuator.h>
#include <yars/util/macros.h>
#include <math.h>

#define ANGULAR        123400
#define VELOCITY       123500
#define FORCE          123600
#define FORCE_VELOCITY 123700
#define FORCE_ANGULAR  123800

#define MAP(min, max, value) ((value > max)?max:(value<min)?min:value)
#define SET(a, b)            if(b >= 0.0) a(b)
#define FACTOR(a)            ((fabs(a) > 0.5)?1:(fabs(a)<0.01)?0:fabs(a))
#define SCALE(a)             MIN((3.0/180.0 * M_PI), fabs(a)) / (3.0/180.0 * M_PI)

HingeActuator::HingeActuator(DataHingeActuator *data, Robot *robot)
  : Actuator("HingeActuator", data->source(), data->destination(), robot)
{
  _data            = data;
  _hingeConstraint = NULL;
  _angle           = 0.0;
  _lastAngle       = 0.0;
  _isActive        = true;
  _friction         = _data->friction();
  _hasFriction      = (_friction > 0.0);

  if(_data->mode() == "passive") _isActive = false;

  _isVisualised = Data::instance()->current()->screens()->visualiseJoints();

  __initHinge();

  switch(_data->controlType())
  {
    case DATA_ACTUATOR_CONTROL_ANGULAR:        _hingeType = ANGULAR;        break;
    case DATA_ACTUATOR_CONTROL_VELOCITY:       _hingeType = VELOCITY;       break;
    case DATA_ACTUATOR_CONTROL_FORCE:          _hingeType = FORCE;          break;
    case DATA_ACTUATOR_CONTROL_FORCE_VELOCITY: _hingeType = FORCE_VELOCITY; break;
    case DATA_ACTUATOR_CONTROL_FORCE_ANGULAR:  _hingeType = FORCE_ANGULAR;  break;
  }

  for(int i = 0; i < 6; i++)
  {
    _hingeConstraint->setParam(BT_CONSTRAINT_STOP_ERP, 1.0, i);
    _hingeConstraint->setParam(BT_CONSTRAINT_STOP_CFM, 0.0, i);
  }

  _parameter = _data->parameter();
}

HingeActuator::~HingeActuator()
{
  if(_hingeConstraint != NULL) delete _hingeConstraint;
}

void HingeActuator::prePhysicsUpdate()
{
  if(_isActive)
  {
    switch(_hingeType)
    {
      case ANGULAR:        __processAngularHinge();       break;
      case VELOCITY:       __processVelocityHinge();      break;
      case FORCE:          __processForceHinge();         break;
      case FORCE_VELOCITY: __processForceVelocityHinge(); break;
      case FORCE_ANGULAR:  __processForceAngularHinge(); break;
    }
  }
  else
  {
    if(_hasFriction)
    {
      // _hingeConstraint->enableAngularMotor(true, 0.0, _friction);
      _hingeConstraint->setPoweredLinMotor(true);
      _hingeConstraint->setTargetLinMotorVelocity(0.0);
      _hingeConstraint->setMaxLinMotorForce(_friction);
    }
  }
}

void HingeActuator::postPhysicsUpdate()
{
  // copied from btSliderConstraint, because it's not calculated if limits are not used
  const btVector3 axisA0 = _hingeConstraint->getCalculatedTransformA().getBasis().getColumn(1);
  const btVector3 axisA1 = _hingeConstraint->getCalculatedTransformA().getBasis().getColumn(2);
  const btVector3 axisB0 = _hingeConstraint->getCalculatedTransformB().getBasis().getColumn(1);

  _angle = btAtan2(axisB0.dot(axisA1), axisB0.dot(axisA0));
  _data->setInternalValue(0, _angle);
  // inverted because its with respect to the second body (I guess)
  _angleDiff = _lastAngle - _angle;
  // cout << _angleDiff << " ";
  if(_angleDiff >  M_PI) _angleDiff -= 2.0*M_PI;
  if(_angleDiff < -M_PI) _angleDiff += 2.0*M_PI;

  _data->setCurrentAngularVelocity(_angleDiff * (double)__YARS_GET_SIMULATOR_FREQUENCY);

  // cout << _angleDiff << " ";
  // cout << _angleDiff * (double)__YARS_GET_SIMULATOR_FREQUENCY << endl;

  _lastAngle = _angle;
  if(_isVisualised)
  {
    btTransform  pose = _hingeConstraint->getCalculatedTransformA();
    btVector3    vec  = pose.getOrigin();
    btQuaternion q    = pose.getRotation();
    _data->setCurrentAxisPosition(P3D(vec[0], vec[1], vec[2]));
    _data->setCurrentAxisOrientation(::Quaternion(q.getW(), q.getX(), q.getY(), q.getZ()));
  }
}

DataActuator* HingeActuator::data()
{
  return _data;
}

void HingeActuator::reset()
{
  delete _hingeConstraint;
  __initHinge();
}

btTypedConstraint* HingeActuator::constraint()
{
  return _hingeConstraint;
}

void HingeActuator::__processAngularHinge()
{
  double robotValue = _data->getInternalDesiredValue(0);

  // Limits don't work well in bullet
  if(_data->isDeflectionSet())
  {
    robotValue = _data->deflection().cut(robotValue);
    _angle     = _data->deflection().cut(_angle);
  }


  // inverted because its with respect to the second body (I guess)
  double diff       = _angle - robotValue;
  // double velocity   = _parameter.pid.update(SCALE(diff) * SIGN(diff) * _data->velocity());
  // double velocity   = _parameter.pid.update(SIGN(diff) * _data->velocity());
  double velocity   = SCALE(diff) * SIGN(diff) * _data->velocity();
  if(fabs(diff) < 0.0001) velocity = 0.0;

  double force = _data->force();

  // if(_parameter.forceScaling > 0.0)
  // {
    // force = force * MAX(0.01, fabs(tanh(_parameter.forceScaling * diff/M_PI)));
  // }

  // cout << robotValue << " " << diff << " " << force << " " << velocity << endl;

  _hingeConstraint->setMaxAngMotorForce(force);
  _hingeConstraint->setTargetAngMotorVelocity(velocity);
  _data->setAppliedForceAndVelocity(0, force, velocity);
}

void HingeActuator::__processForceAngularHinge()
{
  double force      = fabs(_data->getInternalDesiredValue(0));
  double robotValue = _data->getInternalDesiredValue(1);

  if(_data->isDeflectionSet())
  {
    robotValue = _data->deflection().cut(robotValue);
    _angle     = _data->deflection().cut(_angle);
  }

  double diff       = _angle - robotValue;
  double velocity   = SCALE(diff) * SIGN(diff) * _data->velocity();
  if(fabs(diff) < 0.0001) velocity = 0.0;

  _hingeConstraint->setMaxAngMotorForce(force);
  _hingeConstraint->setTargetAngMotorVelocity(velocity);
  _data->setAppliedForceAndVelocity(0, force, velocity);
}

void HingeActuator::__processVelocityHinge()
{
  // double velocity = _parameter.pid.update(_data->getInternalDesiredValue(0));
  double velocity = _data->getInternalDesiredValue(0);
  double force    = _data->force();

  // cout << "setting velocity to " << velocity << " and force to " << _data->force() << endl;

  _hingeConstraint->setMaxAngMotorForce(force);
  _hingeConstraint->setTargetAngMotorVelocity(velocity);

  // if(_data->isActive(0))
  // {
    // cout << _data->name() << ": " << _data->force() << " " << velocity << endl;
  // }
  _data->setAppliedForceAndVelocity(0, force, velocity);
}

void HingeActuator::__processForceHinge()
{
  // Limits don't work well in bullet

  double force    = _data->getInternalDesiredValue(0);
  double velocity = _data->velocity() * tanh(2.0 * force / _data->force());
  force = fabs(force);
  // force          = _parameter.pid.update(force);
  // velocity       = _parameter.pid.update(velocity);
  // to avoid numerical instabilities
  if (fabs(velocity) < 0.0001)               velocity = 0.0f;
  if (fabs(force)    < 0.1 * _data->force()) force    = 0.1 * _data->force();
  // cout << _data->name() << ": force: " << force << " velocity: " << velocity << endl;

  _hingeConstraint->setMaxAngMotorForce(force);
  _hingeConstraint->setTargetAngMotorVelocity(velocity);
  _data->setAppliedForceAndVelocity(0, force, velocity);
}

void HingeActuator::__processForceVelocityHinge()
{
  // Limits don't work well in bullet

  double force    = fabs(_data->getInternalDesiredValue(0));
  double velocity = _data->getInternalDesiredValue(1);
  // force          = _parameter.pid.update(force);
  // velocity       = _parameter.pid.update(velocity);
  // to avoid numerical instabilities
  if (fabs(velocity) < 0.0001) velocity = 0.0f;
  if (fabs(force)    < 0.0001) force    = 0.0f;
  // cout << _data->name() << ": force: " << force << " velocity: " << velocity << endl;

  _hingeConstraint->setMaxAngMotorForce(force);
  _hingeConstraint->setTargetAngMotorVelocity(velocity);
  _data->setAppliedForceAndVelocity(0, force, velocity);
}

void HingeActuator::__initHinge()
{
  _axisPose = _data->pose();


  if(_sourceObject != NULL && _destinationObject != NULL)
  {
    btRigidBody *source      = _sourceObject->rigidBody();
    btRigidBody *destination = _destinationObject->rigidBody();

    ::Quaternion qa = _data->getCurrentAxisOrientation();
    btQuaternion q(qa.x, qa.y, qa.z, qa.w);

    btTransform transformA(q, btVector3(_axisPose.position.x, _axisPose.position.y, _axisPose.position.z));

    btTransform transformB = destination->getWorldTransform().inverse() * transformA;
                transformA = source->getWorldTransform().inverse()      * transformA;

    _hingeConstraint  = new btSliderConstraint(*source, *destination, transformA, transformB, true);
  }
  else
  {
    btRigidBody *source = _sourceObject->rigidBody();

    ::Quaternion qa(_axisPose.orientation);
    btQuaternion q(qa.x, qa.y, qa.z, qa.w);

    btTransform transform = btTransform::getIdentity();
    transform.setOrigin(btVector3(_sourceObject->data()->pose().position.x,
                                  _sourceObject->data()->pose().position.y,
                                  _sourceObject->data()->pose().position.z));

    transform.getBasis().setRotation(q);

    _hingeConstraint = new btSliderConstraint(*source, transform, true);
  }

  _hingeConstraint->setLowerLinLimit(0.0);
  _hingeConstraint->setUpperLinLimit(0.0);
  _hingeConstraint->setPoweredLinMotor(false);

  _hingeConstraint->setPoweredAngMotor(true);

  if(_data->isDeflectionSet())
  {
    _hingeConstraint->setLowerAngLimit(_data->deflection().min);
    _hingeConstraint->setUpperAngLimit(_data->deflection().max);
  }
  else
  {
    _hingeConstraint->setLowerAngLimit(1); // unset limit solving, cause lower > upper
    _hingeConstraint->setUpperAngLimit(0);
  }

  SET(_hingeConstraint->setSoftnessDirAng,      _parameter.softnessDir);
  SET(_hingeConstraint->setRestitutionDirAng,   _parameter.restitutionDir);
  SET(_hingeConstraint->setDampingDirAng,       _parameter.dampingDir);

  SET(_hingeConstraint->setSoftnessLimAng,      _parameter.softnessLim);
  SET(_hingeConstraint->setRestitutionLimAng,   _parameter.restitutionLim);
  SET(_hingeConstraint->setDampingLimAng,       _parameter.dampingLim);

  SET(_hingeConstraint->setSoftnessOrthoAng,    _parameter.softnessOrtho);
  SET(_hingeConstraint->setRestitutionOrthoAng, _parameter.restitutionOrtho);
  SET(_hingeConstraint->setDampingOrthoAng,     _parameter.dampingOrtho);
}
