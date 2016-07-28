#ifndef __CONSOLE_LOGGER_H__
#define __CONSOLE_LOGGER_H__

#include <yars/logging/Logger.h>
#include <yars/configuration/data/DataLoggingConsole.h>

class ConsoleLogger : public Logger
{
  public:
    ConsoleLogger(DataLoggingConsole *data);

    void update();
    void init();
    void close();

  private:
    DataLoggingConsole *_data;

};

#endif // __CONSOLE_LOGGER_H__
