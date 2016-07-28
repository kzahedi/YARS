#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <yars/configuration/data/DataObject.h>

#include <btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>

class Object : public std::vector<Object*>
{
  public:
    Object(DataObject *data, bool isComposite);
    ~Object();

    virtual void reset();

    virtual void postPhysicsUpdate();

    void setCollisionShape(btCollisionShape *collisionShape);
    btCollisionShape* collisionShape();

    void setRigidBody(btRigidBody *rigidBody);
    btRigidBody* rigidBody();

    void setSoftBody(btSoftBody *softBody);
    btSoftBody* softBody();

    DataObject* data();

    bool isCollided();
    void unsetCollided();
    void setCollided(bool collided = true);
    bool isVisible();
    void setIsVisible(bool visible);
    int  type();

    void setMotionState(btMotionState*);

  protected:
    DataObject       *_data;
    btCollisionShape *_collisionShape;
    btRigidBody      *_rigidBody;
    btSoftBody       *_softBody;
    btMotionState    *_motionState;

  private:
    void             __resetPose();
    void             __setInitialValues();

    int              _type;
    bool             _collided;
    bool             _isVisible;
    bool             _isComposite;

};

#endif // __OBJECT_H__

