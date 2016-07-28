#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "LoggingModule.h"

#include <string>
#include <vector>

using namespace std;

class Logger : public std::vector<string>
{
  public:
    Logger();
    virtual ~Logger() { };

    void addModule(LoggingModule *logger);

    virtual void update() = 0;
    virtual void init()   = 0;
    virtual void close()  = 0;

  protected:
    std::vector<LoggingModule*> _modules;
};

#endif // __LOGGER_H__
