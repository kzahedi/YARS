#ifndef __KEY_HANDLER_H__
#define __KEY_HANDLER_H__

#include <yars/configuration/container/KeyboardShortcuts.h>

#include <vector>
#include <map>
#include <functional>

#define YARS_KEY_CODE_OFFSET_SHIFT 1000
#define YARS_KEY_CODE_OFFSET_CTRL  2000
#define YARS_KEY_CODE_OFFSET_ALT   4000


struct KeyEventData
{
  FuncPtrVoidVoid function;
  std::string     description;
};

typedef std::map<int, KeyEventData> KeyEventContainer;

class KeyHandler
{
  public:

    static KeyHandler*  instance();
    static void         registerKeyboardShortcuts();
    static int          handleKeyEvent(bool alt, bool ctrl, bool shift, char c);

    // Lets SdlWindow (or whoever owns the camera) register a function that
    // snaps the camera back to its XML-defined starting position+lookAt.
    // Replaces the Observer hookup that was removed earlier in the
    // refactor. With multiple windows the last registrant wins — fine for
    // now since the keyboard event is single-target anyway.
    static void         setRestoreViewpointCallback(std::function<void()> fn);

    // events
    static void         printKeyCommands();
    static void         toggleReloadOnReset();
    static void         togglePrintoutFPS();
    static void         togglePause();
    static void         reinitAndResetSimulation();
    static void         toggleRealtimeMode();
    static void         activateSingleStep();
    static void         exitSimulation();
    static void         resetSimSpeed();
    static void         decreaseSimSpeed();
    static void         increaseSimSpeed();
    static void         restoreInitialViewpoint();
    static void         toggleDrawMode();
    static void         togglePrintTime();
    static void         toggleSyncedGui();
    static void         openNewWindow();
    static void         toggleCaptureVideo();
    static void         toggleCaptureFrames();

  protected:
    KeyHandler();
    ~KeyHandler();

  private:
    static void               __toggle(bool *a);

    static KeyHandler        *_me;

    static std::vector<int>   _registeredKeyEventCodes;
    static KeyboardShortcuts *_keyboardShortcuts;
};
#endif // __KEY_HANDLER_H__
