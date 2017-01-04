#ifndef __KEY_HANDLER_H__
#define __KEY_HANDLER_H__

#include <yars/configuration/container/KeyboardShortcuts.h>
#include <yars/util/Observable.h>

#include <vector>
#include <map>

#define YARS_KEY_CODE_OFFSET_SHIFT 1000
#define YARS_KEY_CODE_OFFSET_CTRL  2000
#define YARS_KEY_CODE_OFFSET_ALT   4000


struct KeyEventData
{
  FuncPtrVoidVoid function;
  std::string     description;
};

typedef std::map<int, KeyEventData> KeyEventContainer;

class KeyHandler : public Observable
{
  public:

    static KeyHandler*  instance();
    static void         registerKeyboardShortcuts();
    static int          handleKeyEvent(bool alt, bool ctrl, bool shift, char c);

    static void         addObserver(Observer* o);
    static void         removeObserver(Observer* o);
    static void         notifyObservers(ObservableMessage *message);

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
    // static void         closeWindow();

  protected:
    KeyHandler();
    ~KeyHandler();

  private:
    static void               __toggle(bool *a);

    static KeyHandler        *_me;

    static std::vector<int>   _registeredKeyEventCodes;
    static Observable        *_o;
    static KeyboardShortcuts *_keyboardShortcuts;
};
#endif // __KEY_HANDLER_H__
