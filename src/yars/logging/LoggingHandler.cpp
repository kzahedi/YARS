#include <yars/logging/LoggingHandler.h>

#include <yars/logging/LoggingModuleObject.h>

#include <yars/util/stl_macros.h>
#include <yars/view/console/ConsoleView.h>

LoggingHandler::LoggingHandler()
{

}
LoggingHandler::~LoggingHandler()
{
  FOREACH(LoggingModule*, m, _modules)
  {
    delete *m;
  }
}

void LoggingHandler::addModule(LoggingModule *module)
{
  _modules.push_back(module);
}

void LoggingHandler::addLogger(Logger *logger)
{
  bool found = false;
  FOREACH(string, s, *logger)
  {
    found = false;
    FOREACH(LoggingModule*, m, _modules)
    {
      if( (*s) == (*m)->name() )
      {
        logger->addModule(*m);
        found = true;
      }
    }
    if(!found)
    {
      stringstream oss;
      oss << "Logging module: " << *s << " not found.";
      Y_WARN(oss.str());
    }
  }
  _logger.push_back(logger);

}

void LoggingHandler::init()
{
  FOREACH(Logger*, l, _logger) (*l)->init();
}

void LoggingHandler::update()
{
  FOREACH(LoggingModule*, l, _modules) (*l)->update();
  FOREACH(Logger*, l, _logger)         (*l)->update();
}

void LoggingHandler::close()
{
  FOREACH(Logger*, l, _logger)         (*l)->close();
  FOREACH(LoggingModule*, l, _modules) delete (*l);
  FOREACH(Logger*, l, _logger)         delete (*l);
}
