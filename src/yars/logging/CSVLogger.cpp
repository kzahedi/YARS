#include <yars/logging/CSVLogger.h>
#include <yars/util/Timer.h>
#include <yars/configuration/YarsConfiguration.h>

#include <boost/algorithm/string/replace.hpp>

CSVLogger::CSVLogger(DataLoggingCSV *data)
{
  _data = data;
  for(std::vector<string>::iterator s = data->t_begin(); s != data->t_end(); s++)
  {
    push_back(*s);
  }
}

void CSVLogger::init()
{
  _oss.str("");
  std::vector<string> names;
  if(_data->useDate())
  {
    string dateTimeString;
    Timer::getDateTimeString(&dateTimeString);
    _oss << _data->filename() << "-" << dateTimeString << ".csv";
  }
  else
  {
    _oss << _data->filename() << ".csv";
  }
  if(_data->debug()) cout << "opening " << _oss.str().c_str() << endl;
  _output.open(_oss.str().c_str());
  for(std::vector<LoggingModule*>::iterator l = _modules.begin(); l != _modules.end(); l++)
  {
    for(std::vector<string>::iterator v = (*l)->v_begin(); v != (*l)->v_end(); v++)
    {
      names.push_back(*v);
    }
  }
  if(names.size() > 0)
  {
    _output << "# ";
    _oss.str("");
    if(_data->useTimeStep())
    {
      _oss << "time step, ";
    }
    for(int i = 0; i < (int)names.size() - 1; i++)
    {
      _oss << names[i] << ",";
    }
    _oss << names[names.size() - 1] << endl;
    _output << _oss.str();
  }
}

void CSVLogger::update()
{
  if(_data->isLogging(__YARS_GET_STEP) == false) return;
  std::vector<string> values;
  _oss.str("");
  for(std::vector<LoggingModule*>::iterator l = _modules.begin(); l != _modules.end(); l++)
  {
    for(std::vector<string>::iterator v = (*l)->begin(); v != (*l)->end(); v++)
    {
      values.push_back(*v);
    }
  }
  if(values.size() > 0)
  {
    if(_data->useTimeStep())
    {
      _oss << __YARS_GET_STEP << ",";
    }
    for(int i = 0; i < (int)values.size() - 1; i++)
    {
      _oss << boost::replace_all_copy(values[i], " ", ",") << ",";
    }
    _oss << values[values.size() - 1] << endl;
  }
  _output << _oss.str();
}

void CSVLogger::close()
{
  _output.close();
}
