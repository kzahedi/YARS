#include <yars/main/SignalHandler.h>
#include <yars/view/console/ConsoleView.h>
#include <stdlib.h>
#include <iostream>

SignalHandler *SignalHandler::_me = nullptr;
SignalHandler::QuitCallback SignalHandler::_quitCallback;

SignalHandler *SignalHandler::instance()
{
  if (_me == nullptr)
  {
    _me = new SignalHandler();
  }
  return _me;
}

SignalHandler::SignalHandler()
{
  signal(SIGABRT, sighandler);
  signal(SIGTERM, sighandler);
  signal(SIGINT, sighandler);
}

SignalHandler::~SignalHandler()
{
}

void SignalHandler::sighandler(int signal)
{
  switch (signal)
  {
  case SIGABRT:
  case SIGTERM:
  case SIGINT:
    Y_MESSAGE("Signal caught. Will quit yars.");
    if (_quitCallback)
      _quitCallback();
    exit(0);
    break;
  }
}
