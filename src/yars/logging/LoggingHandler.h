#ifndef __LOGGING_MOUDLE_HANDLER_H__
#define __LOGGING_MOUDLE_HANDLER_H__

#include <yars/logging/LoggingModule.h>
#include <yars/logging/Logger.h>
#include <yars/configuration/data/DataObject.h>
#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataActuator.h>
#include <yars/configuration/data/DataController.h>

#include <string>
#include <vector>
#include <memory>

using namespace std;

class LoggingHandler
{
  public:
    LoggingHandler();
    ~LoggingHandler() = default;

    void init();
    void update();
    void close();
    void reset();

    // Both addModule and addLogger take ownership of the passed pointer.
    void addModule(LoggingModule *module);
    void addLogger(Logger        *logger);

  private:
    std::vector<std::unique_ptr<LoggingModule>> _modules;
    std::vector<std::unique_ptr<Logger>>        _logger;
};

#endif // __LOGGING_MOUDLE_HANDLER_H__
