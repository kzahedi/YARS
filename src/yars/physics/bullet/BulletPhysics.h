#ifndef __BULLET_PHYSICS_H__
#define __BULLET_PHYSICS_H__

#include <yars/physics/PhysicsInterface.h>
#include <yars/physics/bullet/World.h>
#include <yars/physics/bullet/Environment.h>
#include <yars/physics/bullet/Robots.h>

// #include "odephysics/base/Collision.h"

class BulletPhysics : public PhysicsInterface
{
public:
  BulletPhysics();
  ~BulletPhysics();

  void init();
  void close();
  void reset();
  void step();
  bool isQuit();
  bool isReset();

private:
  void __deinit();
  void __initWorld();

  bool _quit;
  bool _reset;
  bool _resetCalled;

  World *_world;
  Environment *_environment;
  Robots *_robots;
};
#endif // __PHYSICS_INTERFACE_H__
