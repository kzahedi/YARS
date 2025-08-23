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



void YarsPhysicsControl::notify(ObservableMessage *message)
{
  Y_DEBUG("YarsPhysicsControl: caught message with type %d and text \"%s\"", message->type(), message->string().c_str());
  switch(message->type())
  {
    case __M_INIT:
      _model->initialisePhysics();
      break;
    case __M_NEXT_STEP:
      if(!__YARS_GET_USE_PAUSE || (__YARS_GET_USE_PAUSE && __YARS_GET_USE_SINGLE_STEP))
      {
        // pthread_mutex_lock(&_updateMutex);
        _model->performOneSimulationStep();
        __YARS_SET_USE_SINGLE_STEP(false); // wait for next 'S' in case of pause
        // pthread_mutex_unlock(&_updateMutex);
      }
      else
      {
#ifndef _MSC_VER
        usleep(100);
#else // _MSC_VER
        cout << "Pause not supported in windows version" << endl;
#endif // _MSC_VER
      }
      break;
    case __M_RESET:
      Y_DEBUG("YarsPhysicsControl: processing reset");
      // pthread_mutex_lock(&_updateMutex);
      _model->reset();
      // pthread_mutex_unlock(&_updateMutex);
      break;
    case __M_QUIT:
      delete _model;
      break;
  }
}

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
  delete _model;
}
