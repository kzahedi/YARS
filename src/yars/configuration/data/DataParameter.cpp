#include "DataParameter.h"

#include <stdio.h>
#include <stdlib.h>


DataParameter::DataParameter(DataNode *parent)
  : DataNode(parent)
{

}

void DataParameter::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_PARAMETER))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_PARAMETER))
  {
    element->set(YARS_STRING_NAME, _name);
    element->set(YARS_STRING_VALUE, _value);
  }
}

string DataParameter::name()
{
  return _name;
}

string DataParameter::value()
{
  return _value;
}

int DataParameter::intValue()
{
  return atoi(_value.c_str());
}

bool DataParameter::boolValue()
{
  return _value == "true";
}

yReal DataParameter::realValue()
{
  return (yReal)atof(_value.c_str());
}

void DataParameter::createXsd()
{

}

DataParameter *DataParameter::copy()
{
  DataParameter *copy = new DataParameter(NULL);
  copy->_name = _name;
  copy->_value = _value;
  return copy;
}

void DataParameter::resetTo(const DataParameter *other)
{
  _name  = other->_name;
  _value = other->_value;
}
