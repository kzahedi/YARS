#ifndef __SIGNAL_HANDLER_H__
#define __SIGNAL_HANDLER_H__

#include <signal.h>
#include <functional>

/** \brief Signal handler for YARS.
 *
 * Catches SIGABRT, SIGTERM, SIGINT and calls quit callback.
 */
class SignalHandler
{
public:
  using QuitCallback = std::function<void()>;

  ~SignalHandler();
  static SignalHandler *instance();
  static void sighandler(int signal);

  /** \brief Set callback for quit signals. */
  void setQuitCallback(QuitCallback callback) { _quitCallback = std::move(callback); }

private:
  SignalHandler();
  static SignalHandler *_me;
  static QuitCallback _quitCallback;
};

#endif // __SIGNAL_HANDLER_H__
