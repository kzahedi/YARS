#ifndef __YARS_MAIN_CONTROL_H__
#define __YARS_MAIN_CONTROL_H__

#include <functional>

#include <yars/main/RuntimeControl.h>
#include <yars/main/SignalHandler.h>

#include <yars/configuration/YarsConfiguration.h>

#include <yars/physics/YarsPhysicsControl.h>
#include <yars/physics/YarsPhysicsModel.h>

#include <yars/logging/YarsLoggingControl.h>
#include <yars/logging/YarsLoggingModel.h>

#include <yars/view/console/ConsoleView.h>

namespace yars {

/** \brief This class does the main control.
 *
 * Owns the simulation lifecycle: configuration init, physics setup,
 * logging setup, and the run loop.
 */
class YarsMainControl
{
public:
  YarsMainControl(int argc, char **argv);
  ~YarsMainControl();

  /** \brief Starts the main-loop. Returns when __YARS_GET_EXIT becomes true
     *  or when the iteration cap (RuntimeControl::shouldQuit) is reached. */
  void run();

  /** \brief Register a callback called after each physics step (for GUI sync). */
  void setStepCallback(std::function<void()> callback) { _stepCallback = callback; }

  /** \brief Register a callback called when physics finishes (signals GUI to quit). */
  void setQuitCallback(std::function<void()> callback) { _quitCallback = callback; }

#ifdef USE_CAPTURE_VIDEO
  /** \brief Register a callback to auto-toggle video capture when recording starts/stops. */
  void setToggleCaptureCallback(std::function<void()> callback);
#endif

private:
  void __closeApplication();

  bool _keepOnRunning;
  std::function<void()> _stepCallback;
  std::function<void()> _quitCallback;
  ConsoleView *_cv;
  RuntimeControl *_rtc;
  SignalHandler *_sig;
  YarsConfiguration *_ycc;
  YarsLoggingControl *_ylc;
  YarsLoggingModel *_ylm;
  YarsPhysicsControl *_ypc;
  YarsPhysicsModel *_ypm;
};

} // namespace yars

#endif // __YARS_MAIN_CONTROL_H__
