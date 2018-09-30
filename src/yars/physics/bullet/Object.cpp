#include <yars/physics/bullet/Object.h>
#include <yars/types/Quaternion.h>
#include <yars/types/P3D.h>

Object::Object(DataObject *data)
{
  _data = data;
  _isVisible = true;
  _collisionShape = NULL;
  _rigidBody = NULL;
  _softBody = NULL;
  if (_data != NULL)
    _type = _data->type();
  __setInitialValues();
}

Object::~Object()
{
  delete _collisionShape;
  if (_rigidBody != NULL)
    delete _rigidBody;
  if (_softBody != NULL)
    delete _softBody;

  // delete _motionState;
}

DataObject *Object::data()
{
  return _data;
}

void Object::postPhysicsUpdate()
{
  // cout << _rigidBody->hasContactResponse() << endl;

  if (_data != NULL)
  {
    const btVector3 vel = _rigidBody->getLinearVelocity();
    const btVector3 ang = _rigidBody->getAngularVelocity();
    const btVector3 force = _rigidBody->getTotalForce();
    const btVector3 torque = _rigidBody->getTotalTorque();

    _data->setCurrentForce(force[0], force[1], force[2]);
    _data->setCurrentVelocity(vel[0], vel[1], vel[2]);
    _data->setCurrentTorque(torque[0], torque[1], torque[2]);
    _data->setCurrentAngularVelocity(ang[0], ang[1], ang[2]);
  }
}

bool Object::isCollided()
{
  return _collided;
}

void Object::unsetCollided()
{
  _collided = false;
}

void Object::setCollided(bool collided)
{
  _collided = collided;
}

void Object::reset()
{
  if (_collisionShape != NULL)
  {
    delete _collisionShape;
  }

  if (_rigidBody != NULL)
  {
    delete _rigidBody;
  }

  init();
}

void Object::setCollisionShape(btCollisionShape *shape)
{
  _collisionShape = shape;
}

btCollisionShape *Object::collisionShape()
{
  return _collisionShape;
}

void Object::setRigidBody(btRigidBody *rigidBody)
{
  _rigidBody = rigidBody;
}

btRigidBody *Object::rigidBody()
{
  return _rigidBody;
}

void Object::setSoftBody(btSoftBody *softBody)
{
  _softBody = softBody;
}

btSoftBody *Object::softBody()
{
  return _softBody;
}

void Object::__resetPose()
{
  if (_data == NULL)
    return; // ground plane

  P3D p = _data->pose().position;
  ::Quaternion q = _data->quaternion();

  if (_rigidBody != NULL)
  {
    btVector3 zero(0.0, 0.0, 0.0);
    _rigidBody->setWorldTransform(btTransform(btQuaternion(q.x, q.y, q.z, q.w), btVector3(p.x, p.y, p.z)));
    _rigidBody->setLinearVelocity(zero);
    _rigidBody->setAngularVelocity(zero);
  }

  if (_softBody != NULL)
  {
    _softBody->setWorldTransform(btTransform(btQuaternion(q.x, q.y, q.z, q.w), btVector3(p.x, p.y, p.z)));
  }

  _motionState->setWorldTransform(btTransform(btQuaternion(q.x, q.y, q.z, q.w), btVector3(p.x, p.y, p.z)));
}

void Object::__setInitialValues()
{
  _collided = false;
}

bool Object::isVisible()
{
  return _isVisible;
}

void Object::setIsVisible(bool visible)
{
  _isVisible = visible;
}

int Object::type()
{
  return _type;
}

void Object::setMotionState(btMotionState *motionState)
{
  _motionState = motionState;
}
