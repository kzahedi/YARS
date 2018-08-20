#include <yars/physics/bullet/MuscleActuator.h>
#include <yars/util/macros.h>
#include <math.h>

#define POSITIONAL 123400
#define VELOCITY   123500
#define FORCE      123600

#define MAP(min, max, value) ((value > max)?max:(value<min)?min:value)
#define _SIGN(value)         ((value < 0)?-1:1)
#define SET(a, b)            if(b >= 0.0) a(b)
#define SCALE(a)             (MIN(0.5, fabs(diff)) / 0.5)


MuscleActuator::MuscleActuator(DataMuscleActuator *data, Robot *robot)
  : Actuator("MuscleActuator", data->source(), data->destination(), robot)
{
  _data             = data;
  _muscleConstraint = NULL;
  _lastLength       = 0.0;
  _length           = 0.0;
  _isVisualised     = Data::instance()->current()->screens()->visualiseJoints();
  _parameter        = _data->parameter();
  _velocity         = _data->velocity();
  _maxForce         = _data->force();

  __initSource();
  __initDestination();
  __initSlider();

  // 2. connect slider joint
}

MuscleActuator::~MuscleActuator()
{
  if(_muscleConstraint != NULL) delete _muscleConstraint;
}

void MuscleActuator::prePhysicsUpdate()
{
  _muscleConstraint->setMaxLinMotorForce(1.0);
  _muscleConstraint->setTargetLinMotorVelocity(0.0);
  // _sourceBall->setMaxLinMotorForce(10.0);
  // _sourceBall->setTargetLinMotorVelocity(0.0);
  // _destinationBall->setMaxLinMotorForce(10.0);
  // _destinationBall->setTargetLinMotorVelocity(0.0);
}

void MuscleActuator::postPhysicsUpdate()
{
  // _position = _muscleConstraint->getLinearPos();

  // _data->setCurrentTransitionalVelocity((_position - _lastPosition)
      // * (double)__YARS_GET_SIMULATOR_FREQUENCY);
  // _lastPosition = _position;

  // if(_isVisualised)
  // {
    // btTransform  pose = _muscleConstraint->getCalculatedTransformA();
    // btVector3    vec  = pose.getOrigin();
    // btQuaternion q    = pose.getRotation();
    // _data->setCurrentAxisPosition(P3D(vec[0], vec[1], vec[2]));
    // _data->setCurrentAxisOrientation(::Quaternion(q.getW(), q.getX(), q.getY(), q.getZ()));
  // }
}

DataMuscleActuator* MuscleActuator::data()
{
  return _data;
}

void MuscleActuator::reset()
{
}

btTypedConstraint* MuscleActuator::constraint()
{
  return _muscleConstraint;
}

void MuscleActuator::__initSource()
{
  string sourceAnchorName  = _data->sourceAnchor()->name();
  _srcAnchor               = __find(sourceAnchorName)->rigidBody();
  btRigidBody *destination = _sourceObject->rigidBody();
  cout << "source anchor name: " << sourceAnchorName << endl;

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
  _dstAnchor                   = __find(destinationAnchorName)->rigidBody();
  btRigidBody *destination     = _destinationObject->rigidBody();
  cout << "destination anchor name: " << destinationAnchorName << endl;

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
  Object* srcObject = __find("source anchor");
  Object* dstObject = __find("destination anchor");
  btRigidBody* src  = srcObject->rigidBody();
  btRigidBody* dst  = dstObject->rigidBody();

  btVector3 srcPos  = src->getCenterOfMassPosition();
  btVector3 dstPos  = dst->getCenterOfMassPosition();
  btVector3 dir(dstPos[0], dstPos[1], dstPos[2]);
  dir -= srcPos;
  btVector3 center = (srcPos + dstPos) * btScalar(0.5);

  double xRot = atan2(dstPos[1], dstPos[2]);
  double yRot = atan2(dstPos[0], dstPos[2]);
  double zRot = atan2(dstPos[0], dstPos[1]);

  P3D u(xRot, yRot, zRot);

  ::Quaternion qq(u);

  btQuaternion q(qq.x, qq.y, qq.z, qq.w);

  btTransform global(q, center);

  btTransform localInSrc = src->getWorldTransform().inverse() * global;
  btTransform localInDst = dst->getWorldTransform().inverse() * global;

  _muscleConstraint  = new btSliderConstraint(*src, *dst, localInSrc, localInDst, true);
  _muscleConstraint->setUpperLinLimit(0.5);
  _muscleConstraint->setLowerLinLimit(-0.5);
  _muscleConstraint->setPoweredLinMotor(true);

  // _muscleConstraint->setSoftnessDirLin(0.0);
  // _muscleConstraint->setRestitutionDirLin(0.0);
  // _muscleConstraint->setDampingDirLin(0.0);

  // _muscleConstraint->setSoftnessLimLin(0.0);
  // _muscleConstraint->setRestitutionLimLin(0.0);
  // _muscleConstraint->setDampingLimLin(0.0);

  // _muscleConstraint->setSoftnessOrthoLin(0.0);
  // _muscleConstraint->setRestitutionOrthoLin(0.0);
  // _muscleConstraint->setDampingOrthoLin(0.0);

  for(int i = 0; i < 6; i++)
  {
    _muscleConstraint->setParam(BT_CONSTRAINT_STOP_ERP, 1.0, i);
    _muscleConstraint->setParam(BT_CONSTRAINT_STOP_CFM, 0.0, i);
  }

  _constraints.push_back(_muscleConstraint);
}

void MuscleActuator::__setPoint2Point(btGeneric6DofConstraint *c)
{

  btVector3 linLowerLimit(0.0, 0.0, 0.0);
  btVector3 linUpperLimit(0.0, 0.0, 0.0);

  for(int i = 0; i < 3; i++)
  {
    // c->getRotationalLimitMotor(i)->m_restitution = 0.0;
    c->getRotationalLimitMotor(i)->m_limitSoftness = 0.0;
    c->getRotationalLimitMotor(i)->m_bounce = 0.0;
    c->getRotationalLimitMotor(i)->m_enableMotor = true;
    c->getRotationalLimitMotor(i)->m_targetVelocity = 0.0;
    c->getRotationalLimitMotor(i)->m_maxMotorForce = 0.01;
  }

  c->getTranslationalLimitMotor()->m_enableMotor[0] = false;
  c->getTranslationalLimitMotor()->m_enableMotor[1] = false;
  c->getTranslationalLimitMotor()->m_enableMotor[2] = false;

  c->setLinearLowerLimit(linLowerLimit);
  c->setLinearUpperLimit(linUpperLimit);

  for(int i = 0; i < 3; i++)
  {
    c->getTranslationalLimitMotor()->m_stopERP[i] = 1.0;
    c->getTranslationalLimitMotor()->m_stopCFM[i] = 0.0;
    c->getRotationalLimitMotor(i)->m_stopERP      = 1.0;
    c->getRotationalLimitMotor(i)->m_stopCFM      = 0.0;
  }

  for(int i = 0; i < 6; i++)
  {
    c->setParam(BT_CONSTRAINT_STOP_ERP, 1.0, i);
    c->setParam(BT_CONSTRAINT_STOP_CFM, 0.0, i);
  }

}

