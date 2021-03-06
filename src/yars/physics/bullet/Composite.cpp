#include "Composite.h"
#include "MyCompositeMotionState.h"

#include "Box.h"
#include "Sphere.h"
#include "Capsule.h"
#include "Composite.h"
#include "Cylinder.h"
#include "Ply.h"

Composite::Composite(DataComposite *data) : Object(data)
{
  _data = data;
  clear();
  init();
}

void Composite::init()
{
  btCompoundShape *cShape = new btCompoundShape(_data->g_size());
  MyCompositeMotionState *myMotionState = new MyCompositeMotionState(_data, btTransform::getIdentity());
  setMotionState(myMotionState);

  double mass = 0;
  btScalar *masses = new btScalar[_data->g_size()];
  int index = 0;
  for (DataObjects::iterator oo = _data->g_begin(); oo != _data->g_end(); oo++)
  {
    btCollisionShape *child = NULL;
    mass += (*oo)->physics()->mass();
    masses[index++] = (*oo)->physics()->mass();
    Pose pose = (*oo)->pose();
    btTransform trans(btQuaternion(pose.orientation.x, pose.orientation.y, pose.orientation.z),
                      btVector3(pose.position.x, pose.position.y, pose.position.z));
    btTransform rot = btTransform::getIdentity();
    switch ((*oo)->type())
    {
    case DATA_OBJECT_CAPPED_CYLINDER:
    {
      DataCapsule *capsuleData = (DataCapsule *)(*oo);
      child = new btCapsuleShapeZ(capsuleData->dimension().radius,
                                  capsuleData->dimension().height);
      Capsule *capsule = new Capsule(capsuleData);
      push_back(capsule);
    }
    break;
    case DATA_OBJECT_CYLINDER:
    {
      DataCylinder *cylinderData = (DataCylinder *)(*oo);
      child = new btCylinderShapeZ(btVector3(cylinderData->dimension().radius,
                                             cylinderData->dimension().radius,
                                             cylinderData->dimension().height / 2.0));
      Cylinder *cylinder = new Cylinder(cylinderData);
      push_back(cylinder);
    }
    break;
    case DATA_OBJECT_BOX:
    {
      DataBox *boxData = (DataBox *)(*oo);
      child = new btBoxShape(btVector3(boxData->dimension().width / 2.0f,
                                       boxData->dimension().depth / 2.0f,
                                       boxData->dimension().height / 2.0f));
      Box *box = new Box(boxData);
      push_back(box);
    }
    break;
    case DATA_OBJECT_SPHERE:
    {
      DataSphere *sphereData = (DataSphere *)(*oo);
      child = new btSphereShape(sphereData->radius());
      Sphere *sphere = new Sphere(sphereData);
      push_back(sphere);
    }
    break;
    case DATA_OBJECT_PLY:
    {
      DataPly *plyData = (DataPly *)(*oo);
      Ply *ply = new Ply(plyData);
      child = ply->collisionShape();
      push_back(ply);
    }
    break;
    }
    myMotionState->addObject(*oo);
    cShape->addChildShape(trans, child);
  }

  myMotionState->setShape(cShape);

  btTransform principal;
  btVector3 principalInertia;
  cShape->calculatePrincipalAxisTransform(masses, principal, principalInertia);
  for (int i = 0; i < cShape->getNumChildShapes(); i++)
  {
    btTransform newChildTransform = principal.inverse() * cShape->getChildTransform(i);
    cShape->updateChildTransform(i, newChildTransform);
  }

  btVector3 localInertia(0, 0, 0);
  cShape->calculateLocalInertia(mass, localInertia);
  btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, cShape, localInertia);

  if (_data->physics()->useFriction())
  {
    rbInfo.m_friction = _data->physics()->friction();
  }

  if (_data->physics()->useLinearDamping())
    rbInfo.m_linearDamping = _data->physics()->linearDamping();

  if (_data->physics()->useAngularDamping())
    rbInfo.m_angularDamping = _data->physics()->angularDamping();

  if (_data->physics()->useRollingFriction())
    rbInfo.m_rollingFriction = _data->physics()->rollingFriction();

  if (_data->physics()->useRestitution())
    rbInfo.m_restitution = _data->physics()->restitution();

  _rigidBody = new btRigidBody(rbInfo);
  _rigidBody->setWorldTransform(principal);
  myMotionState->setWorldTransform(principal);
  setCollisionShape(cShape);
  setInitialTransform(principal);
  _rigidBody->setActivationState(DISABLE_DEACTIVATION);
  _rigidBody->setCollisionFlags(_rigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

  for (std::vector<Object *>::iterator o = begin(); o != end(); o++)
  {
    (*o)->setRigidBody(_rigidBody);
  }
}

void Composite::setInitialTransform(btTransform transform)
{
  _initialTransform = transform;
}
