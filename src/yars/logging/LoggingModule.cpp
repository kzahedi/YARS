#include <yars/logging/LoggingModule.h>

#include <yars/util/YarsErrorHandler.h>

LoggingModule::LoggingModule()
{
  _precision = -1;
}

LoggingModule::~LoggingModule()
{

}

void LoggingModule::setPrecision(int precision)
{
  if(precision > _precision) _precision = precision;
  if(_precision >= 0)
  {
    _oss.precision(_precision);
    _oss.setf(ios::fixed,ios::floatfield);
  }
}



void LoggingModule::update()
{
  if(this->size() != _methods.size())
    resize(_methods.size());

  for(int i = 0; i < (int)_methods.size(); i++)
    (*this)[i] = (this->*_methods[i])(_arguments[i]);
}

void LoggingModule::addMethod(LoggingModuleFunctionPtr method, string argument)
{
  _methods.push_back(method);
  _arguments.push_back(argument);
}


string LoggingModule::name()
{
  return _name;
}

std::vector<string>::iterator LoggingModule::v_begin()
{
  return _variables.begin();
}

std::vector<string>::iterator LoggingModule::v_end()
{
  return _variables.end();
}

int LoggingModule::v_size()
{
  return _variables.size();
}

string LoggingModule::getObjectX(string)
{
  _oss.str("");
  _oss << _object->pose().position.x;
  return _oss.str();
}

string LoggingModule::getObjectY(string)
{
  _oss.str("");
  _oss << _object->pose().position.y;
  return _oss.str();
}

string LoggingModule::getObjectZ(string)
{
  _oss.str("");
  _oss << _object->pose().position.z;
  return _oss.str();
}

string LoggingModule::getObjectAlpha(string)
{
  _oss.str("");
  _oss << _object->pose().orientation.x;
  return _oss.str();
}

string LoggingModule::getObjectBeta(string)
{
  _oss.str("");
  _oss << _object->pose().orientation.y;
  return _oss.str();
}

string LoggingModule::getObjectGamma(string)
{
  _oss.str("");
  _oss << _object->pose().orientation.z;
  return _oss.str();
}

string LoggingModule::getSensorInternalValues(string argument)
{
  _oss.str("");
  // for(int i = 0; i < _sensor->dimension(); i++)
  // {
    _oss << _sensor->internalValue((int)argument[0]);
  // }
  return _oss.str();
}

string LoggingModule::getSensorExternalValues(string argument)
{
  _oss.str("");
  // for(int i = 0; i < _sensor->dimension(); i++)
  // {
    _oss << _sensor->externalValue((int)argument[0]);
  // }
  return _oss.str();
}

string LoggingModule::getActuatorExternalValues(string)
{
  _oss.str("");
  for(int i = 0; i < _actuator->dimension(); i++)
  {
    _oss << _actuator->externalValue(i);
  }
  return _oss.str();
}

string LoggingModule::getActuatorInternalValues(string)
{
  _oss.str("");
  for(int i = 0; i < _actuator->dimension(); i++)
  {
    _oss << _actuator->internalValue(i);
  }
  return _oss.str();
}

string LoggingModule::getActuatorDesiredValues(string)
{
  _oss.str("");
  for(int i = 0; i < _actuator->dimension(); i++)
  {
    _oss << _actuator->getExternalDesiredValue(i);
  }
  return _oss.str();
}

string LoggingModule::getActuatorAppliedForce(string)
{
  _oss.str("");
  for(int i = 0; i < _actuator->dimension(); i++)
  {
    _oss << _actuator->getAppliedForce(i);
  }
  return _oss.str();
}

string LoggingModule::getActuatorAppliedVelocity(string)
{
  _oss.str("");
  for(int i = 0; i < _actuator->dimension(); i++)
  {
    _oss << _actuator->getAppliedVelocity(i);
  }
  return _oss.str();
}

string LoggingModule::getControllerVariable(string argument)
{
  _oss.str("");

  if(_controller->getLoggable(argument, _real))
  {
    _oss << _real;
    return _oss.str();
  }

  if(_controller->getLoggable(argument, _integer))
  {
    _oss << _integer;
    return _oss.str();
  }

  if(_controller->getLoggable(argument, _matrix))
  {
    for(int r = 0; r < _matrix.rows() - 1; r++)
    {
      for(int c = 0; c < _matrix.cols(); c++)
      {
        _oss << _matrix(r,c);
      }
    }
    int max_r = _matrix.rows() - 1;
    for(int c = 0; c < _matrix.cols() - 1; c++)
    {
      _oss << _matrix(max_r, c);
    }
    _oss << _matrix(_matrix.rows() - 1, _matrix.cols() - 1);
    return _oss.str();
  }

  if(_controller->getLoggable(argument, _vector))
  {
    for(int i = 0; i < (int)_vector.size() - 1; i++)
    {
      _oss << _vector[i];
    }
    _oss << _vector[_vector.size()-1];
    return _oss.str();
  }

  _oss.str("not found");
  return _oss.str();
}
