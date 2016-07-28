#include "Capsule.h"
#include "MyMotionState.h"

Capsule::Capsule(DataCapsule *data, bool isComposite) : Object(data, isComposite)
{
  Quaternion   q(data->pose().orientation);
  _collisionShape                = new btCapsuleShapeZ(data->dimension().radius, data->dimension().height);
  MyMotionState *myMotionState   = new MyMotionState(data);
  setMotionState(myMotionState);

  btScalar          mass          = data->physics()->mass();
  P3D               localInertia  = data->physics()->centreOfMass();
  btVector3 inertia(localInertia.x, localInertia.y, localInertia.z);
  _collisionShape->calculateLocalInertia(mass, inertia);
  btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, myMotionState, _collisionShape, inertia);

  if(data->physics()->useFriction())
    rigidBodyCI.m_friction = data->physics()->friction();

  if(data->physics()->useLinearDamping())
    rigidBodyCI.m_linearDamping = data->physics()->linearDamping();

  if(data->physics()->useAngularDamping())
    rigidBodyCI.m_angularDamping = data->physics()->angularDamping();

  if(data->physics()->useRollingFriction())
    rigidBodyCI.m_rollingFriction = data->physics()->rollingFriction();

  if(data->physics()->useRestitution())
    rigidBodyCI.m_restitution = data->physics()->restitution();

  _rigidBody = new btRigidBody(rigidBodyCI);
  _rigidBody->setCollisionFlags(_rigidBody->getCollisionFlags() |
      btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
  _rigidBody->setActivationState(DISABLE_DEACTIVATION);
}
