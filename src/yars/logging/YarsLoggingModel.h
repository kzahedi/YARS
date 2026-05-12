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

    // Flushes and closes all CSV / file loggers. Idempotent. Must be called
    // before exit(0) runs in headless mode (see openspec/changes/
    // fix-logging-handler-shutdown-flush).
    void flush();

  private:
    void            __cleanup();
    void            __initLogTraces();
    DataTraces     *_traces;
    LoggingHandler *_loggingHandler;
    bool            _closed = false;
};

#endif // __YARS_LOGGING_MODEL_H__
