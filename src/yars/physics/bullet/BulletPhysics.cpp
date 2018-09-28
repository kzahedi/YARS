#include <yars/physics/bullet/BulletPhysics.h>
#include <yars/view/console/ConsoleView.h>
#include <yars/configuration/YarsConfiguration.h>
#include <yars/physics/bullet/CollisionCallback.h>

BulletPhysics::BulletPhysics()
{
  Y_DEBUG("BulletPhysics constructor called.");
  CollisionCallback::instance();
}

BulletPhysics::~BulletPhysics()
{
  Y_DEBUG("BulletPhysics destructor called.");
}

void BulletPhysics::init()
{
  Y_DEBUG("BulletPhysics init done.");
  _reset = false;
  _quit = false;
  _world = World::instance();
  _environment = new Environment();
  _robots = new Robots();

  __initWorld();

  Y_DEBUG("BulletPhysics init done.");
}

void BulletPhysics::__deinit()
{
}

void BulletPhysics::reset()
{
  Y_DEBUG("BulletPhysics reset called.");

  btVector3 zeroVector(0, 0, 0);
  FOREACH(Object *, o, (*_environment))
  {
    if ((*o)->rigidBody() != NULL)
      _world->removeRigidBody((*o)->rigidBody());
    if ((*o)->rigidBody() != NULL)
      (*o)->rigidBody()->clearForces();
    if ((*o)->rigidBody() != NULL)
      (*o)->rigidBody()->setLinearVelocity(zeroVector);
    if ((*o)->rigidBody() != NULL)
      (*o)->rigidBody()->setAngularVelocity(zeroVector);
  }

  FOREACH(Robot *, m, (*_robots))
  {
    FOREACHF(Object *, o, (*m), ->o_begin(), ->o_end())
    {
      if ((*o) != NULL)
      {
        if ((*o)->rigidBody() != NULL)
          _world->removeRigidBody((*o)->rigidBody());
        cout << "removing object" << endl;
      }
    }

    FOREACHF(Actuator *, a, (*m), ->a_begin(), ->a_end())
    {
      if ((*a)->constraint() != NULL)
        _world->removeConstraint((*a)->constraint());
      if ((*a)->c_size() > 0)
      {
        for (vector<btTypedConstraint *>::iterator i = (*a)->c_begin();
             i != (*a)->c_end(); i++)
        {
          _world->removeConstraint(*i);
        }
      }
    }
  }

  for (int i = 0; i < 100; i++)
    _world->step(__YARS_GET_STEP_SIZE);

  _world->reset();
  _environment->reset();
  _robots->reset();

  __initWorld();

  for (int i = 0; i < 100; i++)
    _world->step(__YARS_GET_STEP_SIZE);
}

void BulletPhysics::close()
{
}

void BulletPhysics::step()
{
  Y_DEBUG("BulletPhysics step called.");
  _environment->prePhysicsUpdate();
  _robots->prePhysicsUpdate();
  _world->step(__YARS_GET_STEP_SIZE);
  _robots->postPhysicsUpdate();
  _environment->postPhysicsUpdate();
  _robots->controllerUpdate();
  _reset = _robots->isReset();
  _quit = _robots->isQuit();
}

bool BulletPhysics::isReset()
{
  return _reset;
}

bool BulletPhysics::isQuit()
{
  return _quit;
}

void BulletPhysics::__initWorld()
{
  int worldMask = 1;
  int worldCollideWith = 0xffffff;

  FOREACH(Object *, o, (*_environment))
  {
    if ((*o)->rigidBody() != NULL)
    {
      _world->addRigidBody((*o)->rigidBody(), worldMask, worldCollideWith);
    }
  }

  int index = 2;
  FOREACH(Robot *, m, (*_robots))
  {
    index++;
    int robotMask = (1 << index);
    int robotCollideWith = ~robotMask;
    if ((*m)->selfCollide())
      robotCollideWith = 0xffffff;
    FOREACHF(Object *, o, (*m), ->o_begin(), ->o_end())
    {
      if ((*o) != NULL)
      {
        if ((*o)->rigidBody() != NULL)
        {
          _world->addRigidBody((*o)->rigidBody(), robotMask, robotCollideWith);
        }
#ifdef USE_SOFT_BODIES
        if ((*o)->softBody() != NULL)
        {
          _world->addSoftBody((*o)->softBody(), robotMask, robotCollideWith);
        }
#endif // USE_SOFT_BODIES
      }
    }

    FOREACHF(Actuator *, a, (*m), ->a_begin(), ->a_end())
    {
      if ((*a)->constraint() != NULL)
      {
        // false -> connected things don't collide
        _world->addConstraint((*a)->constraint(), false);
      }
      if ((*a)->c_size() > 0)
      {
        for (vector<btTypedConstraint *>::iterator i = (*a)->c_begin();
             i != (*a)->c_end(); i++)
        {
          _world->addConstraint(*i, false);
        }
      }
    }
  }
}
