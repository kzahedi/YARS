#include <yars/logging/Logger.h>


#include <iostream>

using namespace std;

Logger::Logger()
{
}


void Logger::addModule(LoggingModule *logger)
{
  _modules.push_back(logger);
}
