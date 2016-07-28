#ifndef __LOGGING_MODULE_OBJECT_H__
#define __LOGGING_MODULE_OBJECT_H__

#include <yars/logging/LoggingModule.h>

#include <yars/configuration/data/DataLoggingObject.h>

#include <vector>
#include <string>

using namespace std;

class LoggingModuleObject : public LoggingModule
{
  public:
    LoggingModuleObject(DataLoggingObject *data);

    void useVariable(string variable);

  private:
    DataLoggingObject *_data;

};

#endif // __LOGGING_MODULE_OBJECT_H__
