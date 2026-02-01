#ifndef __YARS_MAIN_CONTROL_H__
#define __YARS_MAIN_CONTROL_H__

#include <yars/main/RuntimeControl.h>
#include <yars/main/SignalHandler.h>

#include <yars/configuration/YarsConfiguration.h>

#include <yars/physics/YarsPhysicsModel.h>

#include <yars/logging/YarsLoggingModel.h>

#include <yars/view/console/ConsoleView.h>

#include <functional>

/** \brief Main control class for YARS simulation.
 *
 * Coordinates physics, logging, and runtime control.
 * Uses direct function calls instead of Observer pattern.
 */
class YarsMainControl
{
public:
  /** \brief Constructor.
   *
   * \param[in] argc from main()
   * \param[in] argv from main()
   */
  YarsMainControl(int argc, char **argv);

  /** \brief Destructor. */
  ~YarsMainControl();

  /** \brief Starts the main simulation loop. */
  void run();

  /** \brief Request simulation to quit. */
  void requestQuit();

  /** \brief Set callback to quit the view when physics ends. */
  void setViewQuitCallback(std::function<void()> callback);

  /** \brief Set callback for synchronized GUI updates. */
  void setViewSyncCallback(std::function<void()> callback);

private:
  void __init();
  void __step();
  void __reset();
  void __quit();
  void __closeApplication();

  bool _keepOnRunning;
  YarsConfiguration *_ycc;
  YarsPhysicsModel *_ypm;
  YarsLoggingModel *_ylm;
  RuntimeControl *_rtc;
  SignalHandler *_sig;
  std::function<void()> _viewQuitCallback;
  std::function<void()> _viewSyncCallback;
};
#endif // __YARS_MAIN_CONTROL_H__
