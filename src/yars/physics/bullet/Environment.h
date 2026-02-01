#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

#include <yars/configuration/data/Data.h>
#include <yars/physics/bullet/Object.h>

#include <vector>
#include <memory>
#include <btBulletDynamicsCommon.h>

using namespace std;

/* This class creates a plane as ground */

class Environment : public std::vector<std::unique_ptr<Object>>
{
  public:
    Environment();
    ~Environment();

    void reset();
    void prePhysicsUpdate();
    void postPhysicsUpdate();

  private:
    void __create();
    void __init();

    btCollisionShape *_groundShape;
    DataEnvironment  *_data;
};

#endif // __ENVIRONMENT_H__

