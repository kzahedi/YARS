#include "Cylinder.h"
#include "MyMotionState.h"

Cylinder::Cylinder(DataCylinder *data) : Object(data)
{
  _data = data;
  init();
}

void Cylinder::init()
{
  Quaternion q(_data->pose().orientation);
  _collisionShape = new btCylinderShapeZ(btVector3(_data->dimension().radius,
                                                   _data->dimension().radius,
                                                   _data->dimension().height / 2.0));
  MyMotionState *myMotionState = new MyMotionState(_data);
  setMotionState(myMotionState);

  btScalar mass = _data->physics()->mass();
  P3D localInertia = _data->physics()->centreOfMass();
  btVector3 inertia(localInertia.x, localInertia.y, localInertia.z);
  _collisionShape->calculateLocalInertia(mass, inertia);
  btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, myMotionState, _collisionShape, inertia);

  if (_data->physics()->useFriction())
    rigidBodyCI.m_friction = _data->physics()->friction();

  if (_data->physics()->useLinearDamping())
    rigidBodyCI.m_linearDamping = _data->physics()->linearDamping();

  if (_data->physics()->useAngularDamping())
    rigidBodyCI.m_angularDamping = _data->physics()->angularDamping();

  if (_data->physics()->useRollingFriction())
    rigidBodyCI.m_rollingFriction = _data->physics()->rollingFriction();

  if (_data->physics()->useRestitution())
    rigidBodyCI.m_restitution = _data->physics()->restitution();

  _rigidBody = new btRigidBody(rigidBodyCI);
  _rigidBody->setActivationState(DISABLE_DEACTIVATION);
  _rigidBody->setCollisionFlags(_rigidBody->getCollisionFlags() |
                                btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
}
