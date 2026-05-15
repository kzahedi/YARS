#include <yars/physics/YarsPhysicsControl.h>
#include <yars/view/console/ConsoleView.h>

#ifndef _MSC_VER
#  include <unistd.h>
#endif // _MSC_VER


YarsPhysicsControl::YarsPhysicsControl()
{
  pthread_mutexattr_t pthread_mutexattr_default;
  pthread_mutexattr_init(&pthread_mutexattr_default);
  pthread_mutex_init(&_updateMutex, &pthread_mutexattr_default);
}



void YarsPhysicsControl::setModel(YarsPhysicsModel *model)
{
  _model = model;
}



// notify() was the Observer entry point and is gone — YarsMainControl::run()
// now calls init() / step() / reset() / quit() directly.

// Direct control methods
void YarsPhysicsControl::init()
{
  _model->initialisePhysics();
}

void YarsPhysicsControl::step()
{
  if(!__YARS_GET_USE_PAUSE || (__YARS_GET_USE_PAUSE && __YARS_GET_USE_SINGLE_STEP))
  {
    _model->performOneSimulationStep();
    __YARS_SET_USE_SINGLE_STEP(false);
  }
  else
  {
#ifndef _MSC_VER
    usleep(100);
#else
    cout << "Pause not supported in windows version" << endl;
#endif
  }
}

void YarsPhysicsControl::reset()
{
  Y_DEBUG("YarsPhysicsControl: processing reset");
  _model->reset();
}

void YarsPhysicsControl::quit()
{
  // Model lifetime is owned by YarsMainControl; do not delete here.
}
