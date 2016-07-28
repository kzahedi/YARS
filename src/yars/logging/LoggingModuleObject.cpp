#include "LoggingModuleObject.h"

#include <iostream>
#include <sstream>

LoggingModuleObject::LoggingModuleObject(DataLoggingObject *data)
{
  _object = data->object();
  _name   = _object->name();
  _data   = data;
  setPrecision(data->precision());
}

void LoggingModuleObject::useVariable(string variable)
{
  string v = _name + " " + variable;
  _variables.push_back(v);

  if(variable == "x")     addMethod(&LoggingModule::getObjectX);
  if(variable == "y")     addMethod(&LoggingModule::getObjectY);
  if(variable == "z")     addMethod(&LoggingModule::getObjectZ);
  if(variable == "alpha") addMethod(&LoggingModule::getObjectAlpha);
  if(variable == "beta")  addMethod(&LoggingModule::getObjectBeta);
  if(variable == "gamma") addMethod(&LoggingModule::getObjectGamma);
}
