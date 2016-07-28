#ifndef __YARS_MAIN_CONTROL_H__
#define __YARS_MAIN_CONTROL_H__

#include <yars/util/Observable.h>
#include <yars/util/Observer.h>

#include <yars/main/RuntimeControl.h>
#include <yars/main/SignalHandler.h>

#include <yars/configuration/YarsConfiguration.h>

#include <yars/physics/YarsPhysicsControl.h>
#include <yars/physics/YarsPhysicsModel.h>

#include <yars/logging/YarsLoggingControl.h>
#include <yars/logging/YarsLoggingModel.h>

#include <yars/view/console/ConsoleView.h>

/** \brief This class does the main control.
 *
 * It connects the observers and
 * observables, and initiates the run.
 */
class YarsMainControl : public Observable, public Observer
{
  public:
    /** \brief Standard constructor.
     *
     * Takes argc and argv.
     *
     * \param[in] argc, from main(int argc, char **argv)
     * \param[in] argv, from main(int argc, char **argv)
     */
    YarsMainControl(int argc, char **argv);
    /** \brief Desctructor.
     *
     * Closes cleans up
     *
     */
    ~YarsMainControl();

    /** \brief Called from Observables.
     *
     * \param[in] __M_QUIT_CALLED
     * \param[in] __M_RESET
     */
    void notify(ObservableMessage *message);
    /** \brief Starts the main-loop.
     *
     * \param[out] __M_TOGGLE_CAMERA_FOLLOW_MODE
     * \param[out] __M_TOGGLE_SYNCED_GUI
     * \param[out] __M_TOGGLE_CAPTURE_VIDEO
     * \param[out] __M_INIT
     * \param[out] __M_NEXT_STEP
     * \param[out] __M_QUIT_CALLED
     * \param[out] __M_QUIT
     */
    void run();

  private:
    void __closeApplication();

    bool _keepOnRunning;
    ConsoleView              *_cv;
    RuntimeControl           *_rtc;
    SignalHandler            *_sig;
    YarsConfiguration        *_ycc;
    YarsLoggingControl       *_ylc;
    YarsLoggingModel         *_ylm;
    YarsPhysicsControl       *_ypc;
    YarsPhysicsModel         *_ypm;


};
#endif // __YARS_MAIN_CONTROL_H__
