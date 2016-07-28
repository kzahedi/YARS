#ifndef __COLLISION_CALLBACK_H__
#define __COLLISION_CALLBACK_H__

#include "Object.h"

#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <map>

using namespace std;;

class CollisionCallback
{
  public:
    static CollisionCallback *instance();
    // ~CollisionCallback();

    void addObject(Object* object);
    void check(const btCollisionObject* object);

  private:

    static CollisionCallback *_me;
    CollisionCallback();

    map <btCollisionObject*, Object*> _map;
};


#endif // __COLLISION_CALLBACK_H__
