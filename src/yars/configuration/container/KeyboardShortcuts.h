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
    CaptureVideo                    = 0,
    DecreaseSimSpeed                = 1,
    IncreaseSimSpeed                = 2,
    NextFollowMode                  = 3,
    NextFollowable                  = 4,
    OnScreenDisplay                 = 5,
    OnScreenDisplay_ElapsedTime     = 6,
    OnScreenDisplay_FramesPerSecond = 7,
    OpenNewWindow                   = 8,
    CloseWindow                     = 9,
    Pause                           = 10,
    PreviousFollowMode              = 11,
    PreviousFollowable              = 12,
    PrintKeyboardShortcuts          = 13,
    PrintTime                       = 14,
    PrintViewPoint                  = 15,
    Quit                            = 16,
    Realtime                        = 17,
    Reset                           = 18,
    ResetSimSpeed                   = 19,
    RestoreViewpoint                = 20,
    SetWindowSize                   = 21,
    SetWindowTitle                  = 22,
    ShowWindowConfigurationDialog   = 23,
    SingleStep                      = 24,
    ToggleFollowMode                = 25,
    ToggleReloadOnReset             = 26,
    ToggleSyncedGui                 = 27,
    ToggleTextures                  = 28,
    ToggleTraces                    = 29,
    VisualiseAxes                   = 30,
    WriteFrames                     = 31,
    ShootBox                        = 32
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
    KeyboardShortcut closeWindow;
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
