#include "LoggingModuleController.h"

#include <iostream>
#include <sstream>

LoggingModuleController::LoggingModuleController(DataLoggingController *data)
{
  _data = data;
  _controller = _data->controller();
  _name = _data->target();
  setPrecision(data->precision());
}

void LoggingModuleController::useVariable(string variable)
{
  DataController *controller = _data->controller();
  double real;
  int    integer;
  ::Matrix m;
  std::vector<double> v;
  if(controller->getLoggable(variable, real) || controller->getLoggable(variable, integer))
  {
    stringstream oss;
    oss << _data->target() << " " << variable;
    _variables.push_back(oss.str());
  }
  if(controller->getLoggable(variable, v))
  {
    stringstream oss;
    for(int i = 0; i < (int)v.size() - 1; i++)
    {
      oss.str("");
      oss << _data->target() << " " << variable << " " << (i);
      oss << _data->target() << " " << variable << " " << v.size();
      _variables.push_back(oss.str());
    }
  }
  if(controller->getLoggable(variable, m))
  {
    stringstream oss;
    for(int r = 0; r < m.rows(); r++)
    {
      for(int c = 0; c < m.cols(); c++)
      {
        oss.str("");
        oss << _data->target() << " " << variable << "[" << (r) << "," << (c) << "]";
          _variables.push_back(oss.str());
      }
    }
  }
  addMethod(&LoggingModule::getControllerVariable, variable);
}
