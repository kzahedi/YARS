#include <yars/physics/bullet/MuscleActuator.h>
#include <yars/util/macros.h>
#include <math.h>

#define POSITIONAL 123400
#define VELOCITY 123500
#define FORCE 123600

#define MAP(min, max, value) ((value > max) ? max : (value < min) ? min : value)
#define _SIGN(value) ((value < 0) ? -1 : 1)
#define SET(a, b) \
  if (b >= 0.0)   \
  a(b)
#define SCALE(a) (MIN(0.5, fabs(diff)) / 0.5)

MuscleActuator::MuscleActuator(DataMuscleActuator *data, Robot *robot)
    : Actuator("MuscleActuator", data->source(), data->destination(), robot)
{
  _data = data;
  _muscleConstraint = NULL;
  _lastLength = 0.0;
  _length = 0.0;
  _muscleVelocity = 0.0;
  _isVisualised = Data::instance()->current()->screens()->visualiseJoints();
  _parameter = _data->parameter();
  _velocity = _data->velocity();
  _maxForce = _data->force();
  _erp = _data->erp();
  _cfm = _data->cfm();
  _useLengthComponent = _data->lengthComponentUse();
  _useVelocityComponent = _data->velocityComponentUse();
  _lOpt = _data->lengthComponentOptimalLength();
  _w = _data->lengthComponentW();
  _c = _data->lengthComponentC();
  _velMax = _data->velocityComponentMaxVelocity();
  _N = _data->velocityComponentN();
  _K = _data->velocityComponentK();

  __initSource();
  __initDestination();
  __initSlider();
  postPhysicsUpdate();
}

MuscleActuator::~MuscleActuator()
{
  if (_muscleConstraint != NULL)
    delete _muscleConstraint;
}

void MuscleActuator::prePhysicsUpdate()
{

  double fl = 1.0;
  double fv = 1.0;

  if (_useLengthComponent)
  {
    double a = fabs((_length - _lOpt) / (_w * _lOpt));
    // cout << a << " " << (_length - _lOpt) << " / " << (_w * _lOpt) << endl;
    fl = exp(-_c * a * a * a);
  }

  if (_useVelocityComponent)
  {
    if (_muscleVelocity > 0.0)
    {
      fv = (_velMax - _muscleVelocity) / (_velMax - _K * _muscleVelocity);
    }
    else
    {
      fv = _N + (_N - 1.0) * (_velMax - _muscleVelocity) / (-7.56 * _K * _muscleVelocity - _velMax);
    }
  }
  // cout << "use velocity: " << _useVelocityComponent << " " << fv << endl;
  // cout << "length: " << _length << endl;

  double force = _data->getInternalDesiredValue(0);
  // cout << force << endl;
  if (force < 0.0)
    force = 0.0;
  force *= fv * fl;

  _data->setInternalValue(0, force);

  _data->setAppliedForceAndVelocity(0, force, _velocity);
  _data->setLengthComponent(fl);
  _data->setVelocityComponent(fv);
  _data->setMuscleLength(_length);
  _data->setMuscleVelocity(_muscleVelocity);

  _muscleConstraint->setMaxLinMotorForce(force);
  _muscleConstraint->setTargetLinMotorVelocity(_velocity);
}

void MuscleActuator::postPhysicsUpdate()
{
  _lastLength = _length;
  _length = __calculateLength();
  _muscleVelocity = _lastLength - _length;
  _muscleVelocity *= (double)__YARS_GET_SIMULATOR_FREQUENCY;
  _data->setCurrentTransitionalVelocity(_muscleVelocity);
  _data->setMuscleLength(_length);

  // if (_isVisualised)
  // {
  btTransform pose = _muscleConstraint->getCalculatedTransformA();
  btVector3 vec = pose.getOrigin();
  btQuaternion q = pose.getRotation();
  ::Quaternion qq(q.getW(), q.getX(), q.getY(), q.getZ());
  _data->setCurrentAxisPosition(P3D(vec[0], vec[1], vec[2]));
  _data->setCurrentAxisOrientation(qq);

  btVector3 s = _sourceBall->getCalculatedTransformA().getOrigin();
  btVector3 d = _destinationBall->getCalculatedTransformA().getOrigin();
  double x = 0.5 * (s[0] + d[0]);
  double y = 0.5 * (s[1] + d[1]);
  double z = 0.5 * (s[2] + d[2]);
  P3D p(x, y, z);
  _data->setMuscleQuaternion(qq);
  _data->setMusclePosition(p);
}

DataMuscleActuator *MuscleActuator::data()
{
  return _data;
}

void MuscleActuator::reset()
{
}

btTypedConstraint *MuscleActuator::constraint()
{
  return _muscleConstraint;
}

void MuscleActuator::__initSource()
{
  string sourceAnchorName = _data->sourceAnchor()->name();
  _srcAnchor = __find(sourceAnchorName)->rigidBody();
  btRigidBody *destination = _sourceObject->rigidBody();

  P3D v(0.0, 0.0, 1.0);
  ::Quaternion qq(v);
  btQuaternion q(qq.x, qq.y, qq.z, qq.w);

  P3D p = _data->sourceAnchor()->pose().position;
  btVector3 a(p.x, p.y, p.z);
  btTransform global(q, a);

  btTransform localInSrc = _srcAnchor->getWorldTransform().inverse() * global;
  btTransform localInDst = destination->getWorldTransform().inverse() * global;

  _sourceBall = new btGeneric6DofConstraint(*_srcAnchor, *destination, localInSrc, localInDst, true);
  __setPoint2Point(_sourceBall);
  _constraints.push_back(_sourceBall);
}

void MuscleActuator::__initDestination()
{
  string destinationAnchorName = _data->destinationAnchor()->name();
  _dstAnchor = __find(destinationAnchorName)->rigidBody();
  btRigidBody *destination = _destinationObject->rigidBody();

  P3D v(0.0, 0.0, 1.0);
  ::Quaternion qq(v);
  btQuaternion q(qq.x, qq.y, qq.z, qq.w);

  P3D p = _data->destinationAnchor()->pose().position;
  btVector3 a(p.x, p.y, p.z);
  btTransform global(q, a);

  btTransform localInSrc = _dstAnchor->getWorldTransform().inverse() * global;
  btTransform localInDst = destination->getWorldTransform().inverse() * global;

  _destinationBall = new btGeneric6DofConstraint(*_dstAnchor, *destination, localInSrc, localInDst, true);
  __setPoint2Point(_destinationBall);
  _constraints.push_back(_destinationBall);
}

void MuscleActuator::__initSlider()
{
  string sourceAnchorName = _data->sourceAnchor()->name();
  string destinationAnchorName = _data->destinationAnchor()->name();
  Object *srcObject = __find(sourceAnchorName);
  Object *dstObject = __find(destinationAnchorName);
  btRigidBody *src = srcObject->rigidBody();
  btRigidBody *dst = dstObject->rigidBody();

  btVector3 srcPos = src->getCenterOfMassPosition();
  btVector3 dstPos = dst->getCenterOfMassPosition();
  btVector3 centre = (srcPos + dstPos) * btScalar(0.5);
  btVector3 direction = srcPos - dstPos;
  direction.normalize();

  btVector3 rotationAxis(1.0, 0.0, 0.0);

  btScalar angle = direction.angle(rotationAxis);
  btVector3 axis = direction.cross(rotationAxis);
  axis = -axis;
  if (axis[0] > 0.0)
    angle = -angle;

  btQuaternion q(axis, angle);
  btTransform global(q, centre);

  btTransform localInSrc = src->getWorldTransform().inverse() * global;
  btTransform localInDst = dst->getWorldTransform().inverse() * global;

  _muscleConstraint = new btSliderConstraint(*src, *dst, localInSrc, localInDst, true);
  if (_data->isDeflectionSet())
  {
    _muscleConstraint->setLowerLinLimit(_data->deflection().min);
    _muscleConstraint->setUpperLinLimit(_data->deflection().max);
  }
  else
  {
    _muscleConstraint->setLowerLinLimit(1); // unset limit solving
    _muscleConstraint->setUpperLinLimit(0);
  }

  _muscleConstraint->setPoweredLinMotor(true);

  _muscleConstraint->setSoftnessDirLin(_parameter.softnessDir);
  _muscleConstraint->setRestitutionDirLin(_parameter.restitutionDir);
  _muscleConstraint->setDampingDirLin(_parameter.dampingDir);

  _muscleConstraint->setSoftnessLimLin(_parameter.softnessLim);
  _muscleConstraint->setRestitutionLimLin(_parameter.restitutionLim);
  _muscleConstraint->setDampingLimLin(_parameter.dampingLim);

  _muscleConstraint->setSoftnessOrthoLin(_parameter.softnessOrtho);
  _muscleConstraint->setRestitutionOrthoLin(_parameter.restitutionOrtho);
  _muscleConstraint->setDampingOrthoLin(_parameter.dampingOrtho);

  for (int i = 0; i < 6; i++)
  {
    _muscleConstraint->setParam(BT_CONSTRAINT_STOP_ERP, _erp, i);
    _muscleConstraint->setParam(BT_CONSTRAINT_STOP_CFM, _cfm, i);
  }

  _constraints.push_back(_muscleConstraint);

  {
    btTransform pose = _muscleConstraint->getCalculatedTransformA();
    btVector3 vec = pose.getOrigin();
    btQuaternion q = pose.getRotation();
    _data->setCurrentAxisPosition(P3D(vec[0], vec[1], vec[2]));
    _data->setCurrentAxisOrientation(::Quaternion(q.getW(), q.getX(), q.getY(), q.getZ()));
  }
}

void MuscleActuator::__setPoint2Point(btGeneric6DofConstraint *c)
{

  btVector3 linLowerLimit(0.0, 0.0, 0.0);
  btVector3 linUpperLimit(0.0, 0.0, 0.0);

  for (int i = 0; i < 3; i++)
  {
    // c->getRotationalLimitMotor(i)->m_restitution = 0.0;
    c->getRotationalLimitMotor(i)->m_limitSoftness = 1.0;
    c->getRotationalLimitMotor(i)->m_bounce = 0.0;
    if (_data->friction() > 0.0)
    {
      c->getRotationalLimitMotor(i)->m_enableMotor = true;
      c->getRotationalLimitMotor(i)->m_targetVelocity = 0.0;
      c->getRotationalLimitMotor(i)->m_maxMotorForce = _data->friction();
    }
    else
    {
      c->getRotationalLimitMotor(i)->m_enableMotor = false;
      c->getRotationalLimitMotor(i)->m_targetVelocity = 0.0;
      c->getRotationalLimitMotor(i)->m_maxMotorForce = 0.0;
    }
  }

  c->getTranslationalLimitMotor()->m_enableMotor[0] = false;
  c->getTranslationalLimitMotor()->m_enableMotor[1] = false;
  c->getTranslationalLimitMotor()->m_enableMotor[2] = false;

  c->setLinearLowerLimit(linLowerLimit);
  c->setLinearUpperLimit(linUpperLimit);

  for (int i = 0; i < 3; i++)
  {
    c->getTranslationalLimitMotor()->m_stopERP[i] = _erp;
    c->getTranslationalLimitMotor()->m_stopCFM[i] = _cfm;
    c->getRotationalLimitMotor(i)->m_stopERP = _erp;
    c->getRotationalLimitMotor(i)->m_stopCFM = _cfm;
  }

  for (int i = 0; i < 6; i++)
  {
    c->setParam(BT_CONSTRAINT_STOP_ERP, _erp, i);
    c->setParam(BT_CONSTRAINT_STOP_CFM, _cfm, i);
  }
}

double MuscleActuator::__calculateLength()
{
  btVector3 s = _srcAnchor->getCenterOfMassPosition();
  btVector3 d = _dstAnchor->getCenterOfMassPosition();
  btVector3 l = s - d;
  return l.length();
}
