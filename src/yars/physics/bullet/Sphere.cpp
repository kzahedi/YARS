#include "Sphere.h"
#include "MyMotionState.h"

Sphere::Sphere(DataSphere *data, bool isComposite) : Object(data, isComposite)
{
  _collisionShape = new btSphereShape(data->radius());
  MyMotionState    *myMotionState = new MyMotionState(data);
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
  setCollisionShape(_collisionShape);
  _rigidBody->setActivationState(DISABLE_DEACTIVATION);
  _rigidBody->setCollisionFlags(_rigidBody->getCollisionFlags() |
      btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
}
