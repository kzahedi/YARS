#include <yars/main/MainLoopThread.h>

#include <yars/main/YarsMainControl.h>

#include <yars/util/Timer.h>

#include <iostream>

#define __GUI_ASYNC_UPDATE_FRAME_RATE 1.0/5.0 * 1000.0 // 25 fps in millis

MainLoopThread::MainLoopThread(int argc, char **argv)
{
  _argc = argc;
  _argv = argv;
  _lastTime = Timer::getTime();
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
}
