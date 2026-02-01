#ifndef __MAIN_LOOP_THREAD__
#define __MAIN_LOOP_THREAD__

#include <yars/main/YarsMainControl.h>

/** \brief Main loop wrapper for YARS.
 *
 * Creates and runs YarsMainControl.
 */
class MainLoopThread
{
public:
  /** \brief Constructor.
   *
   * \param[in] argc from main()
   * \param[in] argv from main()
   */
  MainLoopThread(int argc, char **argv);

  /** \brief Starts the simulation. */
  void run();

private:
  int _argc;
  char **_argv;
  YarsMainControl *_ymc;
};

#endif //__MAIN_LOOP_THREAD__
