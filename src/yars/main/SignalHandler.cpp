#include <yars/main/SignalHandler.h>
#include <yars/view/console/ConsoleView.h>
#include <yars/configuration/YarsConfiguration.h>
#include <stdlib.h>
#include <iostream>

SignalHandler *SignalHandler::_me = NULL;

SignalHandler* SignalHandler::instance()
{
  if(_me == NULL)
  {
    _me = new SignalHandler();
  }
  return _me;
}

SignalHandler::SignalHandler()
{
  signal(SIGABRT, sighandler);
  signal(SIGTERM, sighandler);
  signal(SIGINT,  sighandler);
}

SignalHandler::~SignalHandler()
{
}

void SignalHandler::sighandler(int signal)
{
  switch(signal)
  {
    case SIGABRT:
    case SIGTERM:
    case SIGINT:
      Y_MESSAGE("Signal caught. Will quit yars.");
      // Set the global exit flag so the main loop sees it on its next
      // iteration. exit(0) below ensures we leave the process even if
      // we're blocked outside the poll site.
      __YARS_SET_EXIT(true);
      exit(0);
      break;
  }
}
