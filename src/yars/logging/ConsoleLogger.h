#ifndef __CONSOLE_LOGGER_H__
#define __CONSOLE_LOGGER_H__

#include <yars/logging/Logger.h>
#include <yars/configuration/data/DataLoggingConsole.h>

class ConsoleLogger : public Logger
{
  public:
    ConsoleLogger(DataLoggingConsole *data);

    void update() override;
    void init() override;
    void close() override;

  private:
    DataLoggingConsole *_data;

};

#endif // __CONSOLE_LOGGER_H__
