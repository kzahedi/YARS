#ifndef __YARS_PHYSICS_CONTROL_H__
#define __YARS_PHYSICS_CONTROL_H__

#include <yars/physics/YarsPhysicsModel.h>

#include <pthread.h>

class YarsPhysicsControl
{
  public:
    YarsPhysicsControl();
    virtual ~YarsPhysicsControl() { };

    void setModel(YarsPhysicsModel *model);

    // Direct control methods
    void init();
    void step();
    void reset();
    void quit();

  private:
    YarsPhysicsModel *_model;
    pthread_mutex_t _updateMutex;
};
#endif // __YARS_PHYSICS_CONTROL_H__

