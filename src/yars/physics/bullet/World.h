#ifndef __BULLET_WORLD_H__
#define __BULLET_WORLD_H__

#define GRAVITY (-9.81)

#include <yars/defines/types.h>
#include <yars/types/P3D.h>

#include <btBulletDynamicsCommon.h>
#ifdef USE_SOFT_BODIES
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <BulletSoftBody/btSoftBodyHelpers.h>
#endif // USE_SOFT_BODIES

#define btSoftRigidDynamicsWorld btSoftRigidDynamicsWorld
// #define btSoftRigidDynamicsWorld btDiscreteDynamicsWorld
class World
{

  public:
    ~World();

    static World *instance();
    static World *reset();
    static P3D rayTest(P3D start, P3D end);

    // proceeds one simulation step
    void step(double stepSize);
    void addRigidBody(btRigidBody *rigidBody, unsigned int mask, unsigned int with);
    void addRigidBody(btRigidBody *rigidBody);
#ifdef USE_SOFT_BODIES
    void addSoftBody(btSoftBody *softBody, unsigned int mask, unsigned int with);
    void addSoftBody(btSoftBody *softBody);
#endif // USE_SOFT_BODIES
    void addCollistionObject(btCollisionObject *collisionObject);
    void removeRigidBody(btRigidBody *rigidBody);
    void addConstraint(btTypedConstraint *constraint, bool selfCollide = false);
    void removeConstraint(btTypedConstraint *constraint);
#ifdef USE_SOFT_BODIES
    btSoftRigidDynamicsWorld *world();
#else  // USE_SOFT_BODIES
    btDiscreteDynamicsWorld *world();
#endif // USE_SOFT_BODIES

  private:
    // btThreadSupportInterface* __createSolverThreadSupport(int maxNumThreads);
    World();

    static World *_me;

    btBroadphaseInterface *_broadphase;
    btCollisionDispatcher *_dispatcher;
    btSequentialImpulseConstraintSolver *_solver;
    bool _collisionShapesInitialised;
#ifdef USE_SOFT_BODIES
    btSoftBodySolver *_softBodySolver;
    btSoftBodyRigidBodyCollisionConfiguration *_collisionConfiguration;
    btSoftRigidDynamicsWorld *_world;
#else  // USE_SOFT_BODIES
    btDefaultCollisionConfiguration *_collisionConfiguration;
    btDiscreteDynamicsWorld *_world;
#endif // USE_SOFT_BODIES
    // btContinuousDynamicsWorld           *_world;
};

#endif // __BULLET_WORLD_H__
