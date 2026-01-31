#include <yars/logging/LoggingHandler.h>

#include <yars/logging/LoggingModuleObject.h>

#include <yars/view/console/ConsoleView.h>

LoggingHandler::LoggingHandler()
{
}

void LoggingHandler::addModule(LoggingModule *module)
{
  _modules.push_back(std::unique_ptr<LoggingModule>(module));
}

void LoggingHandler::addLogger(Logger *logger)
{
  bool found = false;
  for (auto& s : *logger)
  {
    found = false;
    for (auto& m : _modules)
    {
      if (s == m->name())
      {
        logger->addModule(m.get());
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
  _logger.push_back(std::unique_ptr<Logger>(logger));
}

void LoggingHandler::init()
{
  for (auto& l : _logger) l->init();
}

void LoggingHandler::update()
{
  for (auto& m : _modules) m->update();
  for (auto& l : _logger)  l->update();
}

void LoggingHandler::close()
{
  for (auto& l : _logger) l->close();
  // unique_ptr handles cleanup automatically
}

void LoggingHandler::reset()
{
  for (auto& l : _logger) l->close();
  for (auto& l : _logger) l->init();
}
