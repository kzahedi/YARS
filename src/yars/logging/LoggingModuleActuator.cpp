#include "LoggingModuleActuator.h"

#include <iostream>
#include <sstream>

LoggingModuleActuator::LoggingModuleActuator(DataLoggingActuator *data)
{
  _data = data;
  _actuator = data->actuator();
  _name = _actuator->name();
  setPrecision(data->precision());
  std::vector<int> order = _data->order();

  for(std::vector<int>::iterator i = order.begin(); i != order.end(); i++)
  {
    switch(*i)
    {
      case INTERNAL:         __useInternal();        break;
      case EXTERNAL:         __useExternal();        break;
      case DESIRED:          __useDesired();         break;
      case APPLIED_FORCE:    __useAppliedForce();    break;
      case APPLIED_VELOCITY: __useAppliedVelocity(); break;
    }
  }
}

void LoggingModuleActuator::__useAppliedForce()
{
  if(_actuator->dimension() > 1)
  {
    for(int i = 0; i < _actuator->dimension() - 1; i++)
    {
      stringstream oss;
      oss << _actuator->name() << " " << (i+1) << " applied force";
      _variables.push_back(oss.str());
    }
    stringstream oss;
    oss << _actuator->name() << " " << _actuator->dimension() << " applied force";
    _variables.push_back(oss.str());
  }
  else
  {
    stringstream oss;
    oss << _actuator->name() << " applied force";
    _variables.push_back(oss.str());
  }

  addMethod(&LoggingModule::getActuatorAppliedForce);
}

void LoggingModuleActuator::__useAppliedVelocity()
{
  if(_actuator->dimension() > 1)
  {
    for(int i = 0; i < _actuator->dimension() - 1; i++)
    {
      stringstream oss;
      oss << _actuator->name() << " " << (i+1) << " applied velocity";
      _variables.push_back(oss.str());
    }
    stringstream oss;
    oss << _actuator->name() << " " << _actuator->dimension() << " applied velocity";
    _variables.push_back(oss.str());
  }
  else
  {
    stringstream oss;
    oss << _actuator->name() << " applied velocity";
    _variables.push_back(oss.str());
  }

  addMethod(&LoggingModule::getActuatorAppliedVelocity);
}
void LoggingModuleActuator::__useDesired()
{
  if(_actuator->dimension() > 1)
  {
    for(int i = 0; i < _actuator->dimension() - 1; i++)
    {
      stringstream oss;
      oss << _actuator->name() << " " << (i+1) << " desired";
      _variables.push_back(oss.str());
    }
    stringstream oss;
    oss << _actuator->name() << " " << _actuator->dimension() << " desired";
    _variables.push_back(oss.str());
  }
  else
  {
    stringstream oss;
    oss << _actuator->name() << " desired";
    _variables.push_back(oss.str());
  }

  addMethod(&LoggingModule::getActuatorDesiredValues);
}
void LoggingModuleActuator::__useExternal()
{
  if(_actuator->dimension() > 1)
  {
    for(int i = 0; i < _actuator->dimension() - 1; i++)
    {
      stringstream oss;
      oss << _actuator->name() << " " << (i+1) << " external";
      _variables.push_back(oss.str());
    }
    stringstream oss;
    oss << _actuator->name() << " " << _actuator->dimension() << " external";
    _variables.push_back(oss.str());
  }
  else
  {
    stringstream oss;
    oss << _actuator->name() << " external";
    _variables.push_back(oss.str());
  }

  addMethod(&LoggingModule::getActuatorExternalValues);
}

void LoggingModuleActuator::__useInternal()
{
  if(_actuator->dimension() > 1)
  {
    for(int i = 0; i < _actuator->dimension() - 1; i++)
    {
      stringstream oss;
      oss << _actuator->name() << " " << (i+1) << " internal";
      _variables.push_back(oss.str());
    }
    stringstream oss;
    oss << _actuator->name() << " " << _actuator->dimension() << " internal";
    _variables.push_back(oss.str());
  }
  else
  {
    stringstream oss;
    oss << _actuator->name() << " internal";
    _variables.push_back(oss.str());
  }

  addMethod(&LoggingModule::getActuatorInternalValues);
}
