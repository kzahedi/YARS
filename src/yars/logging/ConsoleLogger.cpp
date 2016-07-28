#include <yars/logging/ConsoleLogger.h>

ConsoleLogger::ConsoleLogger(DataLoggingConsole *data)
{
  _data = data;
  for(std::vector<string>::iterator s = data->t_begin(); s != data->t_end(); s++) push_back(*s);
}


void ConsoleLogger::update()
{
  std::vector<string> values;
  for(std::vector<LoggingModule*>::iterator l = _modules.begin(); l != _modules.end(); l++)
  {
    for(std::vector<string>::iterator v = (*l)->begin(); v != (*l)->end(); v++)
    {
      values.push_back(*v);
    }
  }
  if(values.size() > 0)
  {
    for(int i = 0; i < (int)values.size() - 1; i++)
    {
      cout << values[i] << " ";
    }
    cout << values[values.size() - 1] << endl;
  }
}

void ConsoleLogger::init()
{
  std::vector<string> names;
  for(std::vector<LoggingModule*>::iterator l = _modules.begin(); l != _modules.end(); l++)
  {
    for(std::vector<string>::iterator v = (*l)->v_begin(); v != (*l)->v_end(); v++)
    {
      names.push_back(*v);
    }
  }
  if(names.size() > 0)
  {
    for(int i = 0; i < (int)names.size() - 1; i++)
    {
      cout << names[i] << " | ";
    }
    cout << names[names.size() - 1] << endl;
  }
}

void ConsoleLogger::close()
{

}
