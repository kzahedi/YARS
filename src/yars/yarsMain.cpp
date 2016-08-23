#include "main/YarsMainControl.h"

#include "view/console/ConsoleView.h"

#include <iostream>

#ifdef USE_VISUALISATION
//#ifdef SUPPRESS_ALL_OUTPUT
#  define PRINT_START_UP_MESSAGE(a) ;
//#else
//#  define PRINT_START_UP_MESSAGE(a) cout << a << endl;
//#endif

#include "main/MainLoopThread.h"
#include "view/YarsViewControl.h"
#include "view/YarsViewModel.h"

int mainFunction(int argc, char **argv)
{
  YarsMainControl *ymc = new YarsMainControl(argc, argv);

  if(__YARS_GET_USE_VISUALISATION)
  {
    ConsoleView     *cv  = ConsoleView::instance();
    YarsViewControl *yvc = new YarsViewControl();
    YarsViewModel   *yvm = new YarsViewModel();
    yvc->setModel(yvm);
    yvm->addObserver(ymc);
#ifndef SUPPRESS_ALL_OUTPUT
    yvm->addObserver(cv);
#endif // SUPPRESS_ALL_OUTPUT
    ymc->addObserver(yvc);

    boost::thread* pThread = new boost::thread(
        &YarsMainControl::run,     // pointer to member function to execute in thread
        ymc);

    yvm->run();

    pThread->join();
  }
  else
  {
    // YarsMainControl *ymc = new YarsMainControl(argc, argv);
    ymc->run();
  }

  //return app.exec();
  cout << "Good bye." << endl;
  return 0;
}

#else // NO VISUALISATION
int mainFunction(int argc, char **argv)
{
  YarsMainControl *ymc = new YarsMainControl(argc, argv);
  ymc->run();
  return 0;
}
#endif


int main(int argc, char **argv)
{
  return mainFunction(argc, argv);
}
