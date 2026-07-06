#ifndef __SIGNAL_HANDLER_H__
#define __SIGNAL_HANDLER_H__

#include <signal.h>

/** \brief This class implements the YARS signal handler.
 *
 * Installs handlers for SIGTERM/SIGINT (SIGABRT deliberately untrapped: aborts must fail). The handler sets the
 * global exit flag (so YarsMainControl::run() terminates on its next
 * poll) and then exits the process to guarantee we don't hang if the
 * loop is blocked outside the poll site.
 */
class SignalHandler
{
  public:
    ~SignalHandler();
    static SignalHandler* instance();
    static void sighandler(int signal);

  private:
    SignalHandler();
    static SignalHandler* _me;
};
#endif // __SIGNAL_HANDLER_H__
