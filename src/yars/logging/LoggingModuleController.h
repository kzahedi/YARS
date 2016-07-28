#ifndef __LOGGING_MODULE_CONTROLLER_H__
#define __LOGGING_MODULE_CONTROLLER_H__

#include <yars/logging/LoggingModule.h>

#include <yars/configuration/data/DataLoggingController.h>

#include <vector>
#include <string>

using namespace std;

class LoggingModuleController : public LoggingModule
{
  public:
    LoggingModuleController(DataLoggingController *data);

    void useVariable(string variable);

  private:
    DataLoggingController *_data;

};

#endif // __LOGGING_MODULE_CONTROLLER_H__
