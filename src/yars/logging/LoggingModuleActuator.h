#ifndef __LOGGING_MODULE_ACTUATOR_H__
#define __LOGGING_MODULE_ACTUATOR_H__

#include <yars/logging/LoggingModule.h>

#include <yars/configuration/data/DataLoggingActuator.h>

#include <vector>
#include <string>

using namespace std;

class LoggingModuleActuator : public LoggingModule
{
  public:
    LoggingModuleActuator(DataLoggingActuator *data);


  private:
    void __useInternal();
    void __useExternal();
    void __useDesired();
    void __useAppliedForce();
    void __useAppliedVelocity();

    DataLoggingActuator *_data;

};

#endif // __LOGGING_MODULE_ACTUATOR_H__
