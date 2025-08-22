#ifndef __MAIN_LOOP_THREAD__
#define __MAIN_LOOP_THREAD__

#include <yars/util/Observer.h>
#include <yars/main/YarsMainControl.h>

#include <thread>

/** \brief This class is only required if YARS is compiled an run with GUI.
 *
 * Handles the communication between the YARS-core and the Qt-GUI. It translates
 * ObservableMessages to signals and slots, takes care of synchronisation and
 * desynchronised GUI updates.
 *
 */
class MainLoopThread : public Observer
{
  public:
    /** \brief Standard constructor.
     *
     * Takes argc and argv.
     *
     * \param[in] argc, from main(int argc, char **argv)
     * \param[in] argv, from main(int argc, char **argv)
     */
    MainLoopThread(int argc, char **argv);

    /** \brief Starts the thread.
     *
     * Starts the YarsMainControl thread.
     *
     */
    void run();

    /** \brief Catches messages from Observables.
     *
     * Catches messages from Observables and translates them to signals to the
     * GUI.
     *
     * \param[in] message, ObservableMessage message to be send to the GUI.
     *
     */
    void notify(ObservableMessage *m);


  private:
    int    _argc;
    char** _argv;
    //bool   _guiReturned;
    unsigned long _lastTime;
    YarsMainControl *_ymc;
};

#endif //__MAIN_LOOP_THREAD__
