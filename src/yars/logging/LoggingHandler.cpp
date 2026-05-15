#include <yars/logging/LoggingHandler.h>

#include <yars/logging/LoggingModuleObject.h>

#include <yars/view/console/ConsoleView.h>

LoggingHandler::LoggingHandler() = default;

void LoggingHandler::addModule(LoggingModule *module)
{
  _modules.emplace_back(module);
}

void LoggingHandler::addLogger(Logger *logger)
{
  for (const auto &s : *logger)
  {
    bool found = false;
    for (auto &m : _modules)
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
  _logger.emplace_back(logger);
}

void LoggingHandler::init()
{
  for (auto &l : _logger) l->init();
}

void LoggingHandler::update()
{
  for (auto &m : _modules) m->update();
  for (auto &l : _logger)  l->update();
}

void LoggingHandler::close()
{
  for (auto &l : _logger) l->close();
  // unique_ptr handles destruction; no manual delete needed.
}

void LoggingHandler::reset()
{
  for (auto &l : _logger) l->close();
  for (auto &l : _logger) l->init();
}
