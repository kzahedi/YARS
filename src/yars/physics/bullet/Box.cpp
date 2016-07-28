#include "Box.h"
#include "MyMotionState.h"

Box::Box(DataBox *data, bool isComposite) : Object(data, isComposite)
{
  _collisionShape = new btBoxShape(btVector3(data->dimension().width / 2.0f,
                                             data->dimension().depth / 2.0f,
                                             data->dimension().height / 2.0f));
  ::Quaternion q(data->pose().orientation);
  P3D          p = data->pose().position;
  MyMotionState *myMotionState = new MyMotionState(data);
  myMotionState->setWorldTransform(btTransform(btQuaternion(q.x, q.y, q.z, q.w), btVector3(p.x, p.y, p.z)));
  setMotionState(myMotionState);

  btScalar          mass          = data->physics()->mass();
  P3D               localInertia  = data->physics()->centreOfMass();
  btVector3 inertia(localInertia.x, localInertia.y, localInertia.z);
  _collisionShape->calculateLocalInertia(mass, inertia);
  btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, myMotionState, _collisionShape, inertia);

  if(data->physics()->useFriction())
    rigidBodyCI.m_friction = data->physics()->friction();
  else
    rigidBodyCI.m_friction = 1.0;

  if(data->physics()->useLinearDamping())
    rigidBodyCI.m_linearDamping = data->physics()->linearDamping();

  if(data->physics()->useAngularDamping())
    rigidBodyCI.m_angularDamping = data->physics()->angularDamping();

  if(data->physics()->useRollingFriction())
    rigidBodyCI.m_rollingFriction = data->physics()->rollingFriction();

  if(data->physics()->useRestitution())
    rigidBodyCI.m_restitution = data->physics()->restitution();

  _rigidBody = new btRigidBody(rigidBodyCI);
  _rigidBody->setActivationState(DISABLE_DEACTIVATION);
  _rigidBody->setCollisionFlags(_rigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
}

