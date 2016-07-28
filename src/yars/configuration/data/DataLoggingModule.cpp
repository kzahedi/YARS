#include "DataLoggingModule.h"

DataLoggingModule::DataLoggingModule()
{
  _delimiter = " ";
}

DataLoggingModule::~DataLoggingModule()
{
}


std::vector<string>::iterator DataLoggingModule::t_begin()
{
  return _targets.begin();
}
std::vector<string>::iterator DataLoggingModule::t_end()
{
  return _targets.end();
}

int DataLoggingModule::t_size()
{
  return _targets.size();
}

string DataLoggingModule::delimiter()
{
  return _delimiter;
}
