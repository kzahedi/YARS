#ifndef __LOGGING_MODULE_SENSOR_H__
#define __LOGGING_MODULE_SENSOR_H__

#include <yars/logging/LoggingModule.h>

#include <yars/configuration/data/DataLoggingSensor.h>

#include <vector>
#include <string>

using namespace std;

class LoggingModuleSensor : public LoggingModule
{
  public:
    LoggingModuleSensor(DataLoggingSensor *data);


  private:
    void __useInternal(int index);
    void __useExternal(int index);

    DataLoggingSensor *_data;

};

#endif // __LOGGING_MODULE_SENSOR_H__
