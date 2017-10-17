#ifndef __YARS_ERROR_HANDLER_H__
#define __YARS_ERROR_HANDLER_H__

#include <string>
#include <sstream>

#include "Observable.h"
#include "ObservableMessage.h"
#include "YarsException.h"


using namespace std;


class YarsErrorHandler : public stringstream, public Observable
{
  public:
    static YarsErrorHandler* instance();

    static void push() noexcept(false);
    static void push(const char *string, ...);
    static void push(string message);

    string message();

  private:
    YarsErrorHandler();

    static YarsErrorHandler *_me;

};
#endif // __YARS_ERROR_HANDLER_H__
