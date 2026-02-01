#ifndef __KEY_HANDLER_H__
#define __KEY_HANDLER_H__

#include <yars/configuration/container/KeyboardShortcuts.h>

#include <vector>
#include <map>
#include <functional>

#define YARS_KEY_CODE_OFFSET_SHIFT 1000
#define YARS_KEY_CODE_OFFSET_CTRL 2000
#define YARS_KEY_CODE_OFFSET_ALT 4000

struct KeyEventData
{
  FuncPtrVoidVoid function;
  std::string description;
};

typedef std::map<int, KeyEventData> KeyEventContainer;

/** \brief Keyboard handler for YARS GUI.
 *
 * Handles keyboard shortcuts and uses callbacks for quit and video capture.
 */
class KeyHandler
{
public:
  using QuitCallback = std::function<void()>;
  using VideoCaptureCallback = std::function<void()>;
  using ViewpointResetCallback = std::function<void()>;
  using NewWindowCallback = std::function<void()>;

  static KeyHandler *instance();
  static void registerKeyboardShortcuts();
  static int handleKeyEvent(bool alt, bool ctrl, bool shift, char c);

  /** \brief Set callback for quit requests. */
  void setQuitCallback(QuitCallback callback) { _quitCallback = std::move(callback); }

  /** \brief Set callback for video capture toggle. */
  void setVideoCaptureCallback(VideoCaptureCallback callback) { _videoCaptureCallback = std::move(callback); }

  /** \brief Set callback for viewpoint reset. */
  void setViewpointResetCallback(ViewpointResetCallback callback) { _viewpointResetCallback = std::move(callback); }

  /** \brief Set callback for new window. */
  void setNewWindowCallback(NewWindowCallback callback) { _newWindowCallback = std::move(callback); }

  // Key event handlers
  static void printKeyCommands();
  static void toggleReloadOnReset();
  static void togglePrintoutFPS();
  static void togglePause();
  static void reinitAndResetSimulation();
  static void toggleRealtimeMode();
  static void activateSingleStep();
  static void exitSimulation();
  static void resetSimSpeed();
  static void decreaseSimSpeed();
  static void increaseSimSpeed();
  static void restoreInitialViewpoint();
  static void toggleDrawMode();
  static void togglePrintTime();
  static void toggleSyncedGui();
  static void openNewWindow();
  static void toggleCaptureVideo();
  static void toggleCaptureFrames();

protected:
  KeyHandler();
  ~KeyHandler();

private:
  static void __toggle(bool *a);

  static KeyHandler *_me;
  static std::vector<int> _registeredKeyEventCodes;
  static KeyboardShortcuts *_keyboardShortcuts;
  static QuitCallback _quitCallback;
  static VideoCaptureCallback _videoCaptureCallback;
  static ViewpointResetCallback _viewpointResetCallback;
  static NewWindowCallback _newWindowCallback;
};

#endif // __KEY_HANDLER_H__
