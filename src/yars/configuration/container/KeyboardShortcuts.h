#ifndef __KEYBOARD_SHORTCUTS_H__
#define __KEYBOARD_SHORTCUTS_H__

#include "KeyboardShortcut.h"

#include <string>
#include <vector>
#include <iostream>

using namespace std;

namespace YarsKeyFunction
{
  enum Keys
  {
    NoKeyFound                      = -1,
    CaptureVideo                    = 1,
    DecreaseSimSpeed                = 3,
    IncreaseSimSpeed                = 5,
    NextFollowMode                  = 6,
    NextFollowable                  = 7,
    OnScreenDisplay                 = 8,
    OnScreenDisplay_ElapsedTime     = 9,
    OnScreenDisplay_FramesPerSecond = 10,
    OpenNewWindow                   = 11,
    Pause                           = 12,
    PreviousFollowMode              = 13,
    PreviousFollowable              = 14,
    PrintKeyboardShortcuts          = 15,
    PrintTime                       = 16,
    PrintViewPoint                  = 17,
    Quit                            = 18,
    Realtime                        = 19,
    Reset                           = 20,
    ResetSimSpeed                   = 21,
    RestoreViewpoint                = 22,
    SetWindowSize                   = 23,
    SetWindowTitle                  = 24,
    ShowWindowConfigurationDialog   = 25,
    SingleStep                      = 26,
    ToggleFollowMode                = 27,
    ToggleReloadOnReset             = 28,
    ToggleSyncedGui                 = 29,
    ToggleTextures                  = 30,
    ToggleTraces                    = 32,
    VisualiseAxes                   = 33,
    WriteFrames                     = 34,
    ShootBox                        = 35
  };
}

class KeyboardShortcuts
{
  public:
    KeyboardShortcuts();

    void process();
    KeyboardShortcut* get(bool alt, bool ctrl, bool shift, char c);

    KeyboardShortcut quit;
    KeyboardShortcut reset;
    KeyboardShortcut pause;
    KeyboardShortcut realtime;
    KeyboardShortcut printTime;
    KeyboardShortcut singleStep;
    KeyboardShortcut restoreViewpoint;
    KeyboardShortcut decreaseSimSpeed;
    KeyboardShortcut increaseSimSpeed;
    KeyboardShortcut resetSimSpeed;
    KeyboardShortcut toggleReloadOnReset;
    KeyboardShortcut printKeyboardShortcuts;
    KeyboardShortcut captureVideo;

    KeyboardShortcut writeFrames;
    KeyboardShortcut visualiseAxes;
    KeyboardShortcut openNewWindow;
    KeyboardShortcut setWindowTitle;
    KeyboardShortcut showWindowConfigurationDialog;
    KeyboardShortcut setWindowSize;
    KeyboardShortcut onScreenDisplay_FramesPerSecond;
    KeyboardShortcut onScreenDisplay_ElapsedTime;
    KeyboardShortcut onScreenDisplay;
    KeyboardShortcut toggleTextures;
    KeyboardShortcut printViewPoint;
    KeyboardShortcut toggleFollowMode;
    KeyboardShortcut toggleTraces;
    KeyboardShortcut previousFollowable;
    KeyboardShortcut nextFollowable;
    KeyboardShortcut previousFollowMode;
    KeyboardShortcut nextFollowMode;
    KeyboardShortcut shootBox;

    void toString(string *s);

    // KeyboardShortcut toggleDrawMode;
  private:

    void __initialiseSimKeyboardShortcut(KeyboardShortcut* ks);

    std::vector<KeyboardShortcut*> _keys;
};

#endif // __KEYBOARD_SHORTCUTS_H__
