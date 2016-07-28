#include <yars/logging/Logger.h>

#include <yars/util/stl_macros.h>

#include <iostream>

using namespace std;

Logger::Logger()
{
}


void Logger::addModule(LoggingModule *logger)
{
  _modules.push_back(logger);
}
