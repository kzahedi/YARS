#include "DataSignalPeriodic.h"

# define YARS_STRING_MODULE (char*)"module"
# define YARS_STRING_KEY    (char*)"key"
# define YARS_STRING_NAME   (char*)"name"

DataSignalPeriodic::DataSignalPeriodic(DataNode *parent)
  : DataSignal(parent)
{

}

string DataSignalPeriodic::name()
{
  return _name;
}

string DataSignalPeriodic::module()
{
  return _module;
}



void DataSignalPeriodic::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_PERIODIC_SIGNAL))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_PERIODIC_SIGNAL))
  {
    element->set(YARS_STRING_NAME,   _name);
    element->set(YARS_STRING_MODULE, _module);
  }
  if(element->opening(YARS_STRING_PARAMETER))
  {
    DataParameter *parameter = new DataParameter(this);
    parameter->add(element);
    current = parameter;
    push_back(parameter);
  }
}

DataSignalPeriodic* DataSignalPeriodic::_copy()
{
  DataSignalPeriodic *copy = new DataSignalPeriodic(NULL);
  copy->_name = _name;
  copy->_module = _module;
  for(std::vector<DataParameter*>::iterator i = begin(); i != end(); i++)
  {
    copy->push_back((*i)->copy());
  }
  return copy;
}
