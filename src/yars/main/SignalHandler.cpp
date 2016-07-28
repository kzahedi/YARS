#include <yars/main/SignalHandler.h>
#include <yars/view/console/ConsoleView.h>
#include <stdlib.h>
#include <iostream>

Observable    *SignalHandler::_observable = new Observable();
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
  delete _observable;
}

void SignalHandler::sighandler(int signal)
{
  switch(signal)
  {
    case SIGABRT:
    case SIGTERM:
    case SIGINT:
      Y_MESSAGE("Signal caught. Will quit yars.");
      _observable->notifyObservers(_m_signal_handler_activated);
      exit(0);
      break;
  }
}

void SignalHandler::addObserver(Observer *o)
{
  _observable->addObserver(o);
}

void SignalHandler::removeObserver(Observer *o)
{
  _observable->addObserver(o);
}

void SignalHandler::notifyObservers(ObservableMessage *m)
{
  cout << "SignalHandler: " << m->type() << endl;
  _observable->notifyObservers(m);
}

