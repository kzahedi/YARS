#include <yars/configuration/container/KeyboardShortcuts.h>

#include <yars/defines/keyboard_shortcuts.h>

#include <sstream>

using namespace std;

KeyboardShortcuts::KeyboardShortcuts()
{
  quit.keyCode                                = __KEYBOARD_OPTIONS_QUIT_KEY;
  quit.description                            = __KEYBOARD_OPTIONS_QUIT_KEY_DESCRIPTION;
  quit.id                                     = YarsKeyFunction::Quit;
  _keys.push_back(&quit);

  reset.keyCode                               = __KEYBOARD_OPTIONS_RESET_KEY;
  reset.description                           = __KEYBOARD_OPTIONS_RESET_KEY_DESCRIPTION;
  reset.id                                    = YarsKeyFunction::Reset;
  _keys.push_back(&reset);

  pause.keyCode                               = __KEYBOARD_OPTIONS_PAUSE_KEY;
  pause.description                           = __KEYBOARD_OPTIONS_PAUSE_KEY_DESCRIPTION;
  pause.id                                    = YarsKeyFunction::Pause;
  _keys.push_back(&pause);

  realtime.keyCode                            = __KEYBOARD_OPTIONS_REALTIME_KEY;
  realtime.description                        = __KEYBOARD_OPTIONS_REALTIME_KEY_DESCRIPTION;
  realtime.id                                 = YarsKeyFunction::Realtime;
  _keys.push_back(&realtime);

  singleStep.keyCode                          = __KEYBOARD_OPTIONS_SINGLE_STEP_KEY;
  singleStep.description                      = __KEYBOARD_OPTIONS_SINGLE_STEP_KEY_DESCRIPTION;
  singleStep.id                               = YarsKeyFunction::SingleStep;
  _keys.push_back(&singleStep);

  restoreViewpoint.keyCode                    = __KEYBOARD_OPTIONS_RESTORE_VIEWPOINT_KEY;
  restoreViewpoint.description                = __KEYBOARD_OPTIONS_RESTORE_VIEWPOINT_KEY_DESCRIPTION;
  restoreViewpoint.id                         = YarsKeyFunction::RestoreViewpoint;
  _keys.push_back(&restoreViewpoint);

  printTime.keyCode                           = __KEYBOARD_OPTIONS_PRINT_TIME_KEY;
  printTime.description                       = __KEYBOARD_OPTIONS_PRINT_TIME_KEY_DESCRIPTION;
  printTime.id                                = YarsKeyFunction::PrintTime;
  _keys.push_back(&printTime);

  resetSimSpeed.keyCode                       = __KEYBOARD_OPTIONS_RESET_SIMULATION_SPEED_KEY;
  resetSimSpeed.description                   = __KEYBOARD_OPTIONS_RESET_SIMULATION_SPEED_KEY_DESCRIPTION;
  resetSimSpeed.id                            = YarsKeyFunction::ResetSimSpeed;
  _keys.push_back(&resetSimSpeed);

  decreaseSimSpeed.keyCode                    = __KEYBOARD_OPTIONS_DECREASE_SIMULATION_SPEED_KEY;
  decreaseSimSpeed.description                = __KEYBOARD_OPTIONS_DECREASE_SIMULATION_SPEED_KEY_DESCRIPTION;
  decreaseSimSpeed.id                         = YarsKeyFunction::DecreaseSimSpeed;
  _keys.push_back(&decreaseSimSpeed);

  increaseSimSpeed.keyCode                    = __KEYBOARD_OPTIONS_INCREASE_SIMULATION_SPEED_KEY;
  increaseSimSpeed.description                = __KEYBOARD_OPTIONS_INCREASE_SIMULATION_SPEED_KEY_DESCRIPTION;
  increaseSimSpeed.id                         = YarsKeyFunction::IncreaseSimSpeed;
  _keys.push_back(&increaseSimSpeed);

  printKeyboardShortcuts.keyCode              = __KEYBOARD_OPTIONS_PRINT_KEYBOARD_SHORT_CUTS_KEY;
  printKeyboardShortcuts.description          = __KEYBOARD_OPTIONS_PRINT_KEYBOARD_SHORT_CUTS_KEY_DESCRIPTION;
  _keys.push_back(&printKeyboardShortcuts);
  printKeyboardShortcuts.id                   = YarsKeyFunction::PrintKeyboardShortcuts;

  toggleReloadOnReset.keyCode                 = __KEYBOARD_OPTIONS_RELOAD_ON_RESET_KEY;
  toggleReloadOnReset.description             = __KEYBOARD_OPTIONS_RELOAD_ON_RESET_KEY_DESCRIPTION;
  _keys.push_back(&toggleReloadOnReset);
  toggleReloadOnReset.id                      = YarsKeyFunction::ToggleReloadOnReset;

  captureVideo.keyCode                        = __KEYBOARD_OPTIONS_CAPTURE_VIDEO_KEY;
  captureVideo.description                    = __KEYBOARD_OPTIONS_CAPTURE_VIDEO_KEY_DESCRIPTION;
  captureVideo.id                             = YarsKeyFunction::CaptureVideo;
  _keys.push_back(&captureVideo);

  writeFrames.keyCode                         = __KEYBOARD_OPTIONS_WRITE_FRAMES_KEY;
  writeFrames.description                     = __KEYBOARD_OPTIONS_WRITE_FRAMES_DESCRIPTION;
  writeFrames.id                              = YarsKeyFunction::WriteFrames;;
  _keys.push_back(&writeFrames);

  visualiseAxes.keyCode                       = __KEYBOARD_OPTIONS_VISUALISE_AXES_KEY;
  visualiseAxes.description                   = __KEYBOARD_OPTIONS_VISUALISE_AXES_DESCRIPTION;
  visualiseAxes.id                            = YarsKeyFunction::VisualiseAxes;
  _keys.push_back(&visualiseAxes);

  openNewWindow.keyCode                       = __KEYBOARD_OPTIONS_OPEN_NEW_WINDOW_KEY;
  openNewWindow.description                   = __KEYBOARD_OPTIONS_OPEN_NEW_WINDOW_DESCRIPTION;
  openNewWindow.id                            = YarsKeyFunction::OpenNewWindow;
  _keys.push_back(&openNewWindow);

  closeWindow.keyCode                         = __KEYBOARD_OPTIONS_CLOSE_WINDOW_KEY;
  closeWindow.description                     = __KEYBOARD_OPTIONS_CLOSE_WINDOW_DESCRIPTION;
  closeWindow.id                              = YarsKeyFunction::CloseWindow;
  _keys.push_back(&closeWindow);

  setWindowTitle.keyCode                      = __KEYBOARD_OPTIONS_SET_WINDOW_TITLE_KEY;
  setWindowTitle.description                  = __KEYBOARD_OPTIONS_SET_WINDOW_TITLE_DESCRIPTION;
  setWindowTitle.id                           = YarsKeyFunction::SetWindowTitle;
  _keys.push_back(&setWindowTitle);

  showWindowConfigurationDialog.keyCode       = __KEYBOARD_OPTIONS_SHOW_WINDOW_CONFIGURATION_DIALOG_KEY;
  showWindowConfigurationDialog.description   = __KEYBOARD_OPTIONS_SHOW_WINDOW_CONFIGURATION_DIALOG_DESCRIPTION;
  showWindowConfigurationDialog.id            = YarsKeyFunction::ShowWindowConfigurationDialog;
  _keys.push_back(&showWindowConfigurationDialog);

  setWindowSize.keyCode                       = __KEYBOARD_OPTIONS_SET_WINDOW_SIZE_KEY;
  setWindowSize.description                   = __KEYBOARD_OPTIONS_SET_WINDOW_SIZE_DESCRIPTION;
  setWindowSize.id                            = YarsKeyFunction::SetWindowSize;
  _keys.push_back(&setWindowSize);

  onScreenDisplay_FramesPerSecond.keyCode     = __KEYBOARD_OPTIONS_ON_SCREEN_DISPLAY_FRAMES_PER_SECOND_KEY;
  onScreenDisplay_FramesPerSecond.description = __KEYBOARD_OPTIONS_ON_SCREEN_DISPLAY_FRAMES_PER_SECOND_DESCRIPTION;
  onScreenDisplay_FramesPerSecond.id          = YarsKeyFunction::OnScreenDisplay_FramesPerSecond;
  _keys.push_back(&onScreenDisplay_FramesPerSecond);

  onScreenDisplay_ElapsedTime.keyCode         = __KEYBOARD_OPTIONS_ON_SCREEN_DISPLAY_ELAPSED_TIME_KEY;
  onScreenDisplay_ElapsedTime.description     = __KEYBOARD_OPTIONS_ON_SCREEN_DISPLAY_ELAPSED_TIME_DESCRIPTION;
  onScreenDisplay_ElapsedTime.id              = YarsKeyFunction::OnScreenDisplay_ElapsedTime;
  _keys.push_back(&onScreenDisplay_ElapsedTime);

  onScreenDisplay.keyCode                     = __KEYBOARD_OPTIONS_ON_SCREEN_DISPLAY_KEY;
  onScreenDisplay.description                 = __KEYBOARD_OPTIONS_ON_SCREEN_DISPLAY_DESCRIPTION;
  onScreenDisplay.id                          = YarsKeyFunction::OnScreenDisplay;
  _keys.push_back(&onScreenDisplay);

  toggleTextures.keyCode                      = __KEYBOARD_OPTIONS_TOGGLE_TEXTURES_KEY;
  toggleTextures.description                  = __KEYBOARD_OPTIONS_TOGGLE_TEXTURES_DESCRIPTION;
  toggleTextures.id                           = YarsKeyFunction::ToggleTextures;
  _keys.push_back(&toggleTextures);

  printViewPoint.keyCode                      = __KEYBOARD_OPTIONS_PRINT_VIEW_POINT_KEY;
  printViewPoint.description                  = __KEYBOARD_OPTIONS_PRINT_VIEW_POINT_DESCRIPTION;
  printViewPoint.id                           = YarsKeyFunction::PrintViewPoint;
  _keys.push_back(&printViewPoint);

  toggleFollowMode.keyCode                    = __KEYBOARD_OPTIONS_TOGGLE_FOLLOW_MODE_KEY;
  toggleFollowMode.description                = __KEYBOARD_OPTIONS_TOGGLE_FOLLOW_MODE_DESCRIPTION;
  toggleFollowMode.id                         = YarsKeyFunction::ToggleFollowMode;
  _keys.push_back(&toggleFollowMode);

  toggleTraces.keyCode                        = __KEYBOARD_OPTIONS_TOGGLE_TRACES_KEY;
  toggleTraces.description                    = __KEYBOARD_OPTIONS_TOGGLE_TRACES_DESCRIPTION;
  toggleTraces.id                             = YarsKeyFunction::ToggleTraces;
  _keys.push_back(&toggleTraces);

  previousFollowable.keyCode                  = __KEYBOARD_OPTIONS_PREVIOUS_FOLLOWABLE_KEY;
  previousFollowable.description              = __KEYBOARD_OPTIONS_PREVIOUS_FOLLOWABLE_DESCRIPTION;
  previousFollowable.id                       = YarsKeyFunction::PreviousFollowable;
  _keys.push_back(&previousFollowable);

  nextFollowable.keyCode                      = __KEYBOARD_OPTIONS_NEXT_FOLLOWABLE_KEY;
  nextFollowable.description                  = __KEYBOARD_OPTIONS_NEXT_FOLLOWABLE_DESCRIPTION;
  nextFollowable.id                           = YarsKeyFunction::NextFollowable;
  _keys.push_back(&nextFollowable);

  previousFollowMode.keyCode                  = __KEYBOARD_OPTIONS_PREVIOUS_FOLLOW_MODE_KEY;
  previousFollowMode.description              = __KEYBOARD_OPTIONS_PREVIOUS_FOLLOW_MODE_DESCRIPTION;
  previousFollowMode.id                       = YarsKeyFunction::PreviousFollowMode;
  _keys.push_back(&previousFollowMode);

  nextFollowMode.keyCode                      = __KEYBOARD_OPTIONS_NEXT_FOLLOW_MODE_KEY;
  nextFollowMode.description                  = __KEYBOARD_OPTIONS_NEXT_FOLLOW_MODE_DESCRIPTION;
  nextFollowMode.id                           = YarsKeyFunction::NextFollowMode;
  _keys.push_back(&nextFollowMode);

  shootBox.keyCode                            = __KEYBOARD_OPTIONS_SHOOT_BOX_KEY;
  shootBox.description                        = __KEYBOARD_OPTIONS_SHOOT_BOX_DESCRIPTION;
  shootBox.id                                 = YarsKeyFunction::ShootBox;
  _keys.push_back(&shootBox);

}

void KeyboardShortcuts::process()
{
  for(std::vector<KeyboardShortcut*>::iterator i = _keys.begin(); i != _keys.end(); i++)
  {
    (*i)->set();
  }
}

void KeyboardShortcuts::toString(string *s)
{
  stringstream oss;
  for(std::vector<KeyboardShortcut*>::iterator i = _keys.begin(); i != _keys.end(); i++)
  {
    string t;
    (*i)->toString(&t);
    oss << t << endl;
  }
  *s = oss.str();
}


KeyboardShortcut* KeyboardShortcuts::get(bool alt, bool ctrl, bool shift, char c)
{
  for(std::vector<KeyboardShortcut*>::iterator i = _keys.begin(); i != _keys.end(); i++)
  {
    bool a = ((*i)->alt          == alt);
    bool b = ((*i)->ctrl         == ctrl);
    bool k = (tolower((*i)->key) == tolower(c));
    bool s = ((*i)->shift        == shift);

    // keep for debugging
    // if(k)
    // {
      // cout << ">       : " << (*i)->description << endl;
      // cout << "key     : " << (*i)->key         << endl;
      // cout << "keyCode : " << (*i)->keyCode     << endl;
      // cout << "alt     : " << (*i)->alt         << " " << alt   << " " << a << endl;
      // cout << "ctrl    : " << (*i)->ctrl        << " " << ctrl  << " " << b << endl;
      // cout << "shift   : " << (*i)->shift       << " " << shift << " " << b << endl;
      // cout << "key     : " << (*i)->key         << " " << c     << " " << k << endl;
      // cout << "id      : " << (*i)->id          << endl;
    // }

    if(a && b && k && s)
    {
      return (*i);
    }
  }
  return NULL;
}
