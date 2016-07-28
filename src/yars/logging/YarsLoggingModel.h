#ifndef __YARS_LOGGING_MODEL_H__
#define __YARS_LOGGING_MODEL_H__

#include <yars/logging/LoggingHandler.h>
#include <yars/configuration/data/DataTraces.h>

#include <vector>

using namespace std;

class YarsLoggingModel
{
  public:
    YarsLoggingModel();
    ~YarsLoggingModel();

    void reset();
    void init();
    void step();

  private:
    void            __cleanup();
    void            __initLogTraces();
    DataTraces     *_traces;
    LoggingHandler *_loggingHandler;
};

#endif // __YARS_LOGGING_MODEL_H__
