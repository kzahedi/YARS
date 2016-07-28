#include "CollisionCallback.h"

extern ContactAddedCallback gContactAddedCallback;

/***************************************************************************
 *                      collusion callback functions                       *
 ***************************************************************************/

#define MAX_FRICTION 1000000.f

inline btScalar calculateCombinedFriction(float friction0,float friction1)
{
  btScalar friction = friction0 * friction1;

  if (friction < -MAX_FRICTION)
    friction = -MAX_FRICTION;
  if (friction > MAX_FRICTION)
    friction = MAX_FRICTION;

  // cout << friction0 << " * " << friction1 << " = " << friction << endl;

  return friction;
}

inline btScalar calculateCombinedRestitution(float restitution0,float restitution1)
{
  return restitution0 * restitution1;
}

static bool CustomMaterialCombinerCallback(
    btManifoldPoint& cp,
    const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
    const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
{
  float friction0    = colObj0Wrap->getCollisionObject()->getFriction();
  float friction1    = colObj1Wrap->getCollisionObject()->getFriction();
  float restitution0 = colObj0Wrap->getCollisionObject()->getRestitution();
  float restitution1 = colObj1Wrap->getCollisionObject()->getRestitution();

  // cout << "friction0: " << friction0 << " friction1: " << friction1 << endl;

  cp.m_combinedFriction    = calculateCombinedFriction(friction0, friction1);
  cp.m_combinedRestitution = calculateCombinedRestitution(restitution0, restitution1);

  CollisionCallback::instance()->check(colObj0Wrap->getCollisionObject());
  CollisionCallback::instance()->check(colObj1Wrap->getCollisionObject());

  return true;
}

/***************************************************************************
 *                             class functions                             *
 ***************************************************************************/


CollisionCallback* CollisionCallback::_me = NULL;

CollisionCallback* CollisionCallback::instance()
{
  gContactAddedCallback = CustomMaterialCombinerCallback;
  if(_me == NULL) _me = new CollisionCallback();
  return _me;
}

CollisionCallback::CollisionCallback()
{ }

void CollisionCallback::addObject(Object *object)
{
  _map.insert(std::pair<btCollisionObject*, Object*>(object->rigidBody(), object));
}


void CollisionCallback::check(const btCollisionObject* object)
{
  map <btCollisionObject*, Object*>::const_iterator it = _map.find((btCollisionObject*)object);
  if(it != _map.end()) it->second->setCollided();
}


