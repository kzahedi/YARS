#include "main/YarsMainControl.h"
#include "view/console/ConsoleView.h"

#include <iostream>
#include <thread>

#ifdef USE_VISUALISATION
#include "view/YarsViewModel.h"

int mainFunction(int argc, char **argv)
{
  YarsMainControl *ymc = new YarsMainControl(argc, argv);

  if (__YARS_GET_USE_VISUALISATION)
  {
    YarsViewModel *yvm = new YarsViewModel();

    // Run physics in a separate thread
    std::thread physicsThread(&YarsMainControl::run, ymc);

    // Run GUI in main thread
    yvm->run();

    physicsThread.join();

    delete yvm;
  }
  else
  {
    ymc->run();
  }

  delete ymc;
  cout << "Good bye." << endl;
  return 0;
}

#else // NO VISUALISATION

int mainFunction(int argc, char **argv)
{
  YarsMainControl *ymc = new YarsMainControl(argc, argv);
  ymc->run();
  delete ymc;
  return 0;
}

#endif

int main(int argc, char **argv)
{
  return mainFunction(argc, argv);
}
