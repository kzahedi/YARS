#include "KeyHandler.h"

#include <yars/configuration/YarsConfiguration.h>

KeyHandler *KeyHandler::_me = nullptr;
KeyboardShortcuts *KeyHandler::_keyboardShortcuts = nullptr;
std::vector<int> KeyHandler::_registeredKeyEventCodes;
KeyHandler::QuitCallback KeyHandler::_quitCallback;
KeyHandler::VideoCaptureCallback KeyHandler::_videoCaptureCallback;
KeyHandler::ViewpointResetCallback KeyHandler::_viewpointResetCallback;
KeyHandler::NewWindowCallback KeyHandler::_newWindowCallback;

KeyHandler *KeyHandler::instance()
{
  if (_me == nullptr)
    _me = new KeyHandler();
  return _me;
}

KeyHandler::KeyHandler()
{
}

KeyHandler::~KeyHandler()
{
}

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
  if (key == nullptr)
    return -1;
  if (key->function != nullptr)
  {
    key->function();
  }
  return key->id;
}

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
  // Ensure GUI is synced before quitting
  if (!__YARS_GET_SYNC_GUI)
  {
    toggleSyncedGui();
  }
  if (_quitCallback)
    _quitCallback();
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

void KeyHandler::restoreInitialViewpoint()
{
  if (_viewpointResetCallback)
    _viewpointResetCallback();
}

void KeyHandler::toggleDrawMode()
{
  // No-op for now - was used for visualization toggle
}

void KeyHandler::toggleCaptureVideo()
{
  if (_videoCaptureCallback)
    _videoCaptureCallback();
}

void KeyHandler::toggleCaptureFrames()
{
  // Toggle frame capture - uses existing capture configuration
  bool b = __YARS_GET_USE_CAPTURE_CL;
  __toggle(&b);
  __YARS_SET_USE_CAPTURE(b);
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
  __YARS_SET_USE_SINGLE_STEP(false);
}

void KeyHandler::toggleReloadOnReset()
{
  // Reload on reset functionality - currently disabled
  // Would require adding useReload accessor to YarsConfiguration
}

void KeyHandler::openNewWindow()
{
  if (_newWindowCallback)
    _newWindowCallback();
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
