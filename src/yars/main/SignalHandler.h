#ifndef __SIGNAL_HANDLER_H__
#define __SIGNAL_HANDLER_H__

#include <yars/util/Observer.h>
#include <yars/util/Observable.h>
#include <yars/util/ObservableMessage.h>

#include <signal.h>

/** \brief This class implements the YARS signal handler.
 */
class SignalHandler
{
  public:
    ~SignalHandler();
    static SignalHandler* instance();
    static void sighandler(int signal);

    static void addObserver(Observer *o);
    static void removeObserver(Observer *o);
    static void notifyObservers(ObservableMessage *m);

  private:
    SignalHandler();
    static SignalHandler* _me;
    static Observable *_observable;
};
#endif // __SIGNAL_HANDLER_H__

