#ifndef __YARS_PHYSICS_MODEL_H__
#define __YARS_PHYSICS_MODEL_H__

#include <yars/util/Observable.h>
#include <yars/physics/bullet/BulletPhysics.h>

class PhysicsInterface;

class YarsPhysicsModel : public Observable
{
  public:
    YarsPhysicsModel();
    ~YarsPhysicsModel();

    // slots
    void initialisePhysics();
    void performOneSimulationStep();
    void performMultipleSimulationSteps(int numberOfSteps);
    void shutdown();
    void reset();

  private:

    PhysicsInterface *_physics;

};
#endif // __YARS_PHYSICS_MODEL_H__


