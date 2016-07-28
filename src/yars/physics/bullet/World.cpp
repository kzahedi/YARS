#include <yars/physics/bullet/World.h>

#include <yars/configuration/YarsConfiguration.h>
#include <yars/configuration/data/Data.h>

#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletSoftBody/btDefaultSoftBodySolver.h>

#include <iostream>
#include <map>

using namespace std;

World* World::_me = NULL;

World* World::instance()
{
  if(_me == NULL)
  {
    _me = new World();
  }
  return _me;
}

World::World()
{
  _collisionShapesInitialised = false;

#ifdef USE_SOFT_BODIES
  _collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
#else // USE_SOFT_BODIES
  _collisionConfiguration = new btDefaultCollisionConfiguration();
#endif // USE_SOFT_BODIES
  _dispatcher             = new btCollisionDispatcher(_collisionConfiguration);
  _broadphase             = new btDbvtBroadphase();
  _solver                 = new btSequentialImpulseConstraintSolver();

#ifdef USE_SOFT_BODIES
  _softBodySolver         = new btDefaultSoftBodySolver();
  _world                  = new btSoftRigidDynamicsWorld(_dispatcher, _broadphase, _solver, _collisionConfiguration, _softBodySolver);
  yReal x = Data::instance()->current()->environment()->gravitation(0);
  yReal y = Data::instance()->current()->environment()->gravitation(1);
  yReal z = Data::instance()->current()->environment()->gravitation(2);
  _world->setGravity(btVector3(x, y, z));
  _world->getWorldInfo().m_gravity.setValue(x, y, z);

  _world->getSolverInfo().m_numIterations = Data::instance()->current()->simulator()->getSolverIterations();
#else // USE_SOFT_BODIES
  _world                  = new btDiscreteDynamicsWorld(_dispatcher, _broadphase, _solver, _collisionConfiguration);
  yReal x = Data::instance()->current()->environment()->gravitation(0);
  yReal y = Data::instance()->current()->environment()->gravitation(1);
  yReal z = Data::instance()->current()->environment()->gravitation(2);

  _world->setGravity(btVector3(x, y, z));
  _world->getSolverInfo().m_numIterations = Data::instance()->current()->simulator()->getSolverIterations();
#endif // USE_SOFT_BODIES

 
  // btThreadSupportInterface* thread = __createSolverThreadSupport(4);
  // _solver                 = new btParallelConstraintSolver(thread);
  // _world          = new btDiscreteDynamicsWorld(_dispatcher, _broadphase, _solver, _collisionConfiguration);
  // _world->setNumTasks(2);

}

World::~World()
{
}

void World::reset()
{
  _solver->reset();
  _world->clearForces();
  _broadphase->resetPool(_dispatcher);
}

void World::step(yReal stepSize)
{
  _world->stepSimulation(__YARS_GET_STEP_SIZE, 10, __YARS_GET_STEP_SIZE);
}

void World::addRigidBody(btRigidBody *rigidBody, unsigned int mask, unsigned int with)
{
  _world->addRigidBody(rigidBody, mask, with);
}

#ifdef USE_SOFT_BODIES
void World::addSoftBody(btSoftBody *softBody, unsigned int mask, unsigned int with)
{
  _world->addSoftBody(softBody, mask, with);
}

void World::addSoftBody(btSoftBody *softBody)
{
  _world->addSoftBody(softBody);
}
#endif // USE_SOFT_BODIES

void World::addRigidBody(btRigidBody *rigidBody)
{
  _world->addRigidBody(rigidBody);
}


void World::addCollistionObject(btCollisionObject *collisionObject)
{
  _world->addCollisionObject(collisionObject,
      btBroadphaseProxy::SensorTrigger, btBroadphaseProxy::AllFilter & ~btBroadphaseProxy::SensorTrigger);
  
  if(!_collisionShapesInitialised)
  {
    _world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());   
    _collisionShapesInitialised = true;
  }
}

void World::removeRigidBody(btRigidBody *rigidBody)
{
  _world->removeRigidBody(rigidBody);
}

void World::addConstraint(btTypedConstraint *constraint, bool selfCollide)
{
  _world->addConstraint(constraint, !selfCollide);
}

void World::removeConstraint(btTypedConstraint *constraint)
{
  _world->removeConstraint(constraint);
}

P3D World::rayTest(P3D start, P3D end)
{
  btVector3 _start(start.x, start.y, start.z);
  btVector3 _end(end.x, end.y, end.z);
  btCollisionWorld::ClosestRayResultCallback rayCallback(_start, _end);
  _me->_world->rayTest(_start, _end, rayCallback);
  btVector3 hit = rayCallback.m_hitPointWorld;
  // cout << " hit: " << hit[0] << " " << hit[1] << " " << hit[2];
  if(rayCallback.hasHit()) return P3D(hit[0], hit[1], hit[2]);
  // cout << " end: " << end.x << " " << end.y << " " << end.z;
  return end;
}

#ifdef USE_SOFT_BODIES
    btSoftRigidDynamicsWorld* World::world()
#else // USE_SOFT_BODIES
    btDiscreteDynamicsWorld* World::world()
#endif // USE_SOFT_BODIES
{
  return _world;
}


