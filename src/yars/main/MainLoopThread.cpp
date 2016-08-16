#include "MainLoopThread.h"

#include "YarsMainControl.h"

#include <yars/util/Timer.h>

#include <iostream>

#define __GUI_ASYNC_UPDATE_FRAME_RATE 1000.0/5.0 * 1000.0 // 25 fps in millis

MainLoopThread::MainLoopThread(int argc, char **argv)
{
  _argc = argc;
  _argv = argv;
  _lastTime = Timer::getTime();
  // for ode 0.11.1 
  setStackSize(2097152);
  _ymc = new YarsMainControl(_argc, _argv);
  _ymc->addObserver(this);
};

void MainLoopThread::run()
{
  _ymc->run();
};

void MainLoopThread::notify(ObservableMessage *message)
{
  Y_DEBUG("MainLoopThread::notify(\"%s\")", message->string().c_str());
  if(!__YARS_GET_USE_VISUALISATION) return; // no need to send and wait for signals
  if(message->type() == __M_TOGGLE_SYNCED_GUI)
  {
    _lastTime = Timer::getTime();
  }

  if(message->type() == __M_QUIT_GUI_CALLED)
  {
    emit observerMessage(message);
    return;
  }

  if(!__YARS_GET_SYNC_GUI && message->type() == __M_INIT)
  {
    __YARS_SET_SYNC_GUI(true);
    _guiReturned = false;
    emit observerMessage(message);
    while(!_guiReturned)
    {
      usleep(10);
    }
    __YARS_SET_SYNC_GUI(false);
    return;
  }

  if(__YARS_GET_SYNC_GUI)
  {
    _guiReturned = false;
    emit observerMessage(message);
    while(!_guiReturned)
    {
      usleep(10);
    }
  }
  else // not synced
  {
    if(_guiReturned && (Timer::getTime() - _lastTime > __GUI_ASYNC_UPDATE_FRAME_RATE))
    {
      _guiReturned = false;
      emit observerMessage(message);
      _lastTime = Timer::getTime();
    }
  }
}

void MainLoopThread::hookUp(QObject *w)
{
  connect(this, SIGNAL(observerMessage(ObservableMessage*)), w, SLOT(notifySlot(ObservableMessage*)));
  connect(w, SIGNAL(guiDoneSignal()), this, SLOT(guiDoneSlot()));
}

void MainLoopThread::guiDoneSlot()
{
  _guiReturned = true;
}
