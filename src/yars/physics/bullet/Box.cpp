#include "Box.h"
#include "MyMotionState.h"

Box::Box(DataBox *data, bool isComposite) : Object(data, isComposite)
{
  _data = data;
  init();
}

void Box::init()
{
  _collisionShape = new btBoxShape(btVector3(_data->dimension().width / 2.0f,
                                             _data->dimension().depth / 2.0f,
                                             _data->dimension().height / 2.0f));
  ::Quaternion q(_data->pose().orientation);
  P3D          p = _data->pose().position;
  MyMotionState *myMotionState = new MyMotionState(_data);
  myMotionState->setWorldTransform(btTransform(btQuaternion(q.x, q.y, q.z, q.w), btVector3(p.x, p.y, p.z)));
  setMotionState(myMotionState);

  btScalar          mass          = _data->physics()->mass();
  P3D               localInertia  = _data->physics()->centreOfMass();
  btVector3 inertia(localInertia.x, localInertia.y, localInertia.z);
  _collisionShape->calculateLocalInertia(mass, inertia);
  btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, myMotionState, _collisionShape, inertia);

  if(_data->physics()->useFriction())
    rigidBodyCI.m_friction = _data->physics()->friction();
  else
    rigidBodyCI.m_friction = 1.0;

  if(_data->physics()->useLinearDamping())
    rigidBodyCI.m_linearDamping = _data->physics()->linearDamping();

  if(_data->physics()->useAngularDamping())
    rigidBodyCI.m_angularDamping = _data->physics()->angularDamping();

  if(_data->physics()->useRollingFriction())
    rigidBodyCI.m_rollingFriction = _data->physics()->rollingFriction();

  if(_data->physics()->useRestitution())
    rigidBodyCI.m_restitution = _data->physics()->restitution();

  _rigidBody = new btRigidBody(rigidBodyCI);
  _rigidBody->setActivationState(DISABLE_DEACTIVATION);
  _rigidBody->setCollisionFlags(_rigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
}

