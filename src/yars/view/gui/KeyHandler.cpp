#include "KeyHandler.h"

#include "configuration/YarsConfiguration.h"

#include <boost/function.hpp>

KeyHandler*        KeyHandler::_me                = NULL;
KeyboardShortcuts* KeyHandler::_keyboardShortcuts = NULL;
Observable*        KeyHandler::_o                 = new Observable();

std::vector<int> KeyHandler::_registeredKeyEventCodes;

KeyHandler* KeyHandler::instance()
{
  if(_me == NULL) _me = new KeyHandler();
  return _me;
}

KeyHandler::KeyHandler()
{
}

KeyHandler::~KeyHandler()
{
  delete _o;
}

//////////////////////////////////////////////////////////////////////
//
//  public functions
//
//////////////////////////////////////////////////////////////////////

void KeyHandler::registerKeyboardShortcuts()
{
  YarsConfiguration *configuration = YarsConfiguration::instance();

  _keyboardShortcuts = configuration->getKeyboardShortcuts();

  _keyboardShortcuts->pause.function                 = &togglePause;
  _keyboardShortcuts->quit.function                  = &exitSimulation;
  _keyboardShortcuts->reset.function                 = &reinitAndResetSimulation;
  //_keyboardShortcuts->printKeyCommands.function      = &printKeyCommands;
  _keyboardShortcuts->toggleReloadOnReset.function   = &toggleReloadOnReset;
  _keyboardShortcuts->realtime.function              = &toggleRealtimeMode;
  _keyboardShortcuts->singleStep.function            = &activateSingleStep;
  _keyboardShortcuts->decreaseSimSpeed.function      = &decreaseSimSpeed;
  _keyboardShortcuts->increaseSimSpeed.function      = &increaseSimSpeed;
  _keyboardShortcuts->resetSimSpeed.function         = &resetSimSpeed;
  _keyboardShortcuts->restoreViewpoint.function      = &restoreInitialViewpoint;
  _keyboardShortcuts->printTime.function             = &togglePrintTime;

  _keyboardShortcuts->captureVideo.function          = NULL;

}


void KeyHandler::addObserver(Observer *o)
{
  _o->addObserver(o);
}

void KeyHandler::removeObserver(Observer *o)
{
  _o->addObserver(o);
}

void KeyHandler::notifyObservers(ObservableMessage *m)
{
  _o->notifyObservers(m);
}

int KeyHandler::handleKeyEvent(bool alt, bool ctrl, bool shift, char c)
{
  KeyboardShortcut *key = _keyboardShortcuts->get(alt, ctrl, shift, c);
  if(key == NULL) return -1;
  if(key->function != NULL)
  {
    key->function();
  }
  return key->id;
}

//////////////////////////////////////////////////////////////////////
//
//  event functions
//
//////////////////////////////////////////////////////////////////////

void KeyHandler::reinitAndResetSimulation()
{
  _o->notifyObservers(_m_reset);
}

void KeyHandler::toggleRealtimeMode()
{
  bool b = __YARS_GET_USE_REAL_TIME;
  __toggle(&b);
  __YARS_SET_USE_REAL_TIME(b);
}

void KeyHandler::activateSingleStep()
{
  __YARS_SET_USE_SINGLE_STEP(true);
  _o->notifyObservers(_m_toggleSingleStep);
}

void KeyHandler::exitSimulation()
{
  // TODO: hack, quitting should also work without syncing
  if(!__YARS_GET_SYNC_GUI)
  {
    toggleSyncedGui(); // so that we can quit nicely
  }
  _o->notifyObservers(_m_quit_called);
}

void KeyHandler::decreaseSimSpeed()
{
  if(!__YARS_GET_USE_REAL_TIME) return;
  __YARS_SET_REAL_TIME_FACTOR(__YARS_GET_REAL_TIME_FACTOR * 2.0);
}

void KeyHandler::resetSimSpeed()
{
  if(!__YARS_GET_USE_REAL_TIME) return;
  __YARS_SET_REAL_TIME_FACTOR(1.0);
}

void KeyHandler::increaseSimSpeed()
{
  if(!__YARS_GET_USE_REAL_TIME) return;
  __YARS_SET_REAL_TIME_FACTOR(__YARS_GET_REAL_TIME_FACTOR * 0.5);
}

void KeyHandler::restoreInitialViewpoint()
{
  _o->notifyObservers(_m_resetInitiailViewpoint);
}

void KeyHandler::toggleDrawMode()
{
  _o->notifyObservers(_m_toggleVisualisation);
}

void KeyHandler::togglePrintTime()
{
  bool b = __YARS_GET_USE_PRINT_TIME_INFORMATION;
  __toggle(&b);
  __YARS_SET_USE_PRINT_TIME_INFORMATION(b);
}

void KeyHandler::togglePause()
{
  bool b = __YARS_GET_USE_PAUSE;
  __toggle(&b);
  __YARS_SET_USE_PAUSE(b);
  __YARS_SET_USE_SINGLE_STEP(false); // will be activated by user
  _o->notifyObservers(_m_togglePause);
}

void KeyHandler::toggleReloadOnReset()
{
  notifyObservers(_m_toggleReload);
}

void KeyHandler::toggleSyncedGui()
{
  bool b = __YARS_GET_SYNC_GUI;
  __toggle(&b);
  __YARS_SET_SYNC_GUI(b);
  _o->notifyObservers(_m_toggleSyncedGui);
}

void KeyHandler::__toggle(bool *a)
{
  *a = !*a;
}


