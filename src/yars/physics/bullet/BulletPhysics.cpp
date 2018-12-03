#include <yars/physics/bullet/BulletPhysics.h>
#include <yars/view/console/ConsoleView.h>
#include <yars/configuration/YarsConfiguration.h>
#include <yars/configuration/data/Data.h>
#include <yars/physics/bullet/CollisionCallback.h>

BulletPhysics::BulletPhysics()
{
  Y_DEBUG("BulletPhysics constructor called.");
  CollisionCallback::instance();
  _resetCalled = false;
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
  _resetCalled = true;
  Y_DEBUG("BulletPhysics reset called.");

  if (_environment != NULL)
    delete _environment;
  if (_robots != NULL)
    delete _robots;

  _world = World::reset();
  Data::instance()->initialise(0);
  _environment = new Environment();
  _robots = new Robots();

  __initWorld();
  _resetCalled = false;
}

void BulletPhysics::close()
{
}

void BulletPhysics::step()
{
  if (_resetCalled == true)
    return;
  Y_DEBUG("BulletPhysics step called.");
  // cout << "BulletPhysics step called." << endl;
  _environment->prePhysicsUpdate();
  _robots->prePhysicsUpdate();
  _world->step(__YARS_GET_STEP_SIZE);
  _robots->postPhysicsUpdate();
  _environment->postPhysicsUpdate();
  if (__YARS_GET_USE_CONTROLLER == true)
  {
    _robots->controllerUpdate();
  }
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
