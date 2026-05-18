#include "KeyHandler.h"

#include <yars/configuration/YarsConfiguration.h>

#include <functional>

KeyHandler *KeyHandler::_me = NULL;
KeyboardShortcuts *KeyHandler::_keyboardShortcuts = NULL;

std::vector<int> KeyHandler::_registeredKeyEventCodes;

KeyHandler *KeyHandler::instance()
{
  if (_me == NULL)
    _me = new KeyHandler();
  return _me;
}

KeyHandler::KeyHandler()
{
}

KeyHandler::~KeyHandler()
{
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

  _keyboardShortcuts->pause.function = &togglePause;
  _keyboardShortcuts->quit.function = &exitSimulation;
  _keyboardShortcuts->reset.function = &reinitAndResetSimulation;
  _keyboardShortcuts->toggleReloadOnReset.function = &toggleReloadOnReset;
  _keyboardShortcuts->realtime.function = &toggleRealtimeMode;
  _keyboardShortcuts->singleStep.function = &activateSingleStep;
  _keyboardShortcuts->decreaseSimSpeed.function = &decreaseSimSpeed;
  _keyboardShortcuts->increaseSimSpeed.function = &increaseSimSpeed;
  _keyboardShortcuts->resetSimSpeed.function = &resetSimSpeed;
  _keyboardShortcuts->restoreViewpoint.function = &restoreInitialViewpoint;
  _keyboardShortcuts->openNewWindow.function = &openNewWindow;
  _keyboardShortcuts->printTime.function = &togglePrintTime;
  _keyboardShortcuts->captureVideo.function = &toggleCaptureVideo;
  _keyboardShortcuts->writeFrames.function = &toggleCaptureFrames;
}

int KeyHandler::handleKeyEvent(bool alt, bool ctrl, bool shift, char c)
{
  KeyboardShortcut *key = _keyboardShortcuts->get(alt, ctrl, shift, c);
  if (key == NULL)
    return -1;
  if (key->function != NULL)
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
  __YARS_SET_RESET_SIMULATION;
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
}

void KeyHandler::exitSimulation()
{
  // Set the global exit flag; YarsMainControl::run() polls it on every
  // iteration.
  __YARS_SET_EXIT(true);
}

void KeyHandler::decreaseSimSpeed()
{
  if (!__YARS_GET_USE_REAL_TIME)
    return;
  __YARS_SET_REAL_TIME_FACTOR(__YARS_GET_REAL_TIME_FACTOR * 2.0);
}

void KeyHandler::resetSimSpeed()
{
  if (!__YARS_GET_USE_REAL_TIME)
    return;
  __YARS_SET_REAL_TIME_FACTOR(1.0);
}

void KeyHandler::increaseSimSpeed()
{
  if (!__YARS_GET_USE_REAL_TIME)
    return;
  __YARS_SET_REAL_TIME_FACTOR(__YARS_GET_REAL_TIME_FACTOR * 0.5);
}

namespace { std::function<void()> g_restoreViewpointCallback; }

void KeyHandler::setRestoreViewpointCallback(std::function<void()> fn)
{
  g_restoreViewpointCallback = std::move(fn);
}

void KeyHandler::restoreInitialViewpoint()
{
  if (g_restoreViewpointCallback) g_restoreViewpointCallback();
}

void KeyHandler::toggleDrawMode()
{
  // No-op (see restoreInitialViewpoint).
}

void KeyHandler::toggleCaptureVideo()
{
  // No-op (see restoreInitialViewpoint).
}

void KeyHandler::toggleCaptureFrames()
{
  // No-op (see restoreInitialViewpoint).
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
}

void KeyHandler::toggleReloadOnReset()
{
  // No-op (see restoreInitialViewpoint).
}

void KeyHandler::openNewWindow()
{
  // No-op (see restoreInitialViewpoint).
}

void KeyHandler::toggleSyncedGui()
{
  bool b = __YARS_GET_SYNC_GUI;
  __toggle(&b);
  __YARS_SET_SYNC_GUI(b);
}

void KeyHandler::__toggle(bool *a)
{
  *a = !*a;
}
