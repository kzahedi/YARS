#include "LoggingModuleSensor.h"

#include <iostream>
#include <sstream>

LoggingModuleSensor::LoggingModuleSensor(DataLoggingSensor *data)
{
  _data                  = data;
  _sensor                = data->sensor();
  _name                  = _sensor->name();
  std::vector<int> order = _data->order();
  setPrecision(data->precision());

  if(_sensor->dimension() > 1)
  {
    for(int index = 0; index < _sensor->dimension(); index++)
    {
      for(std::vector<int>::iterator i = order.begin(); i != order.end(); i++)
      {
        switch(*i)
        {
          case INTERNAL: __useInternal(index); break;
          case EXTERNAL: __useExternal(index); break;
        }
      }
    }
  }
  else
  {
    for(std::vector<int>::iterator i = order.begin(); i != order.end(); i++)
    {
      switch(*i)
      {
        case INTERNAL: __useInternal(-1); break;
        case EXTERNAL: __useExternal(-1); break;
      }
    }
  }
}

void LoggingModuleSensor::__useExternal(int index)
{
  if(index >= 0)
  {
    stringstream oss;
    oss << _sensor->logName(index) << " " << index << " external";
    _variables.push_back(oss.str());
  }
  else
  {
    stringstream oss;
    oss << _sensor->logName(0) << " external";
    _variables.push_back(oss.str());
  }

  char* c = new char[1];
  c[0] = (char)index;
  string s(c);
  addMethod(&LoggingModule::getSensorExternalValues, s);
  delete c;
}

void LoggingModuleSensor::__useInternal(int index)
{
  if(index >= 0)
  {
    stringstream oss;
    oss << _sensor->logName(index) << " " << index << " internal";
    _variables.push_back(oss.str());
  }
  else
  {
    stringstream oss;
    oss << _sensor->logName(0) << " internal";
    _variables.push_back(oss.str());
  }

  char* c = new char[1];
  c[0] = (char)index;
  string s(c);
  addMethod(&LoggingModule::getSensorInternalValues, c);
  delete c;
}
