#include <yars/main/MainLoopThread.h>

MainLoopThread::MainLoopThread(int argc, char **argv)
{
  _argc = argc;
  _argv = argv;
  _ymc = new YarsMainControl(_argc, _argv);
}

void MainLoopThread::run()
{
  _ymc->run();
}
