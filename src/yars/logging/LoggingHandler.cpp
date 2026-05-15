#include <yars/logging/LoggingHandler.h>

#include <yars/logging/LoggingModuleObject.h>

#include <yars/view/console/ConsoleView.h>

LoggingHandler::LoggingHandler()
{

}
LoggingHandler::~LoggingHandler()
{
  for (auto *m : _modules) delete m;
}

void LoggingHandler::addModule(LoggingModule *module)
{
  _modules.push_back(module);
}

void LoggingHandler::addLogger(Logger *logger)
{
  for (const auto &s : *logger)
  {
    bool found = false;
    for (auto *m : _modules)
    {
      if (s == m->name())
      {
        logger->addModule(m);
        found = true;
      }
    }
    if (!found)
    {
      stringstream oss;
      oss << "Logging module: " << s << " not found.";
      Y_WARN(oss.str());
    }
  }
  _logger.push_back(logger);
}

void LoggingHandler::init()
{
  for (auto *l : _logger) l->init();
}

void LoggingHandler::update()
{
  for (auto *l : _modules) l->update();
  for (auto *l : _logger)  l->update();
}

void LoggingHandler::close()
{
  for (auto *l : _logger)  l->close();
  for (auto *l : _modules) delete l;
  for (auto *l : _logger)  delete l;
}

void LoggingHandler::reset()
{
  for (auto *l : _logger) l->close();
  for (auto *l : _logger) l->init();
}
