#include "DataSignalTriggered.h"

# define YARS_STRING_SOURCE (char*)"source"
# define YARS_STRING_MODULE (char*)"module"
# define YARS_STRING_KEY    (char*)"key"

DataSignalTriggered::DataSignalTriggered(DataNode *parent)
  : DataSignal(parent)
{

}

string DataSignalTriggered::name()
{
  return _name;
}

string DataSignalTriggered::module()
{
  return _module;
}



void DataSignalTriggered::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_TRIGGERED_SIGNAL))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_TRIGGERED_SIGNAL))
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

void DataSignalTriggered::createXsd(XsdSpecification *spec)
{
  XsdSequence *triggeredSignal = new XsdSequence(YARS_STRING_TRIGGERED_SIGNAL_DEFINITION);
  triggeredSignal->add(NA(YARS_STRING_NAME,      YARS_STRING_XSD_STRING,           true));
  triggeredSignal->add(NA(YARS_STRING_MODULE,    YARS_STRING_XSD_STRING,           true));
  triggeredSignal->add(NA(YARS_STRING_SOURCE,    YARS_STRING_XSD_STRING,           true));
  triggeredSignal->add(NE(YARS_STRING_PARAMETER, YARS_STRING_PARAMETER_DEFINITION, 0, 1));
  spec->add(triggeredSignal);
}

DataSignalTriggered* DataSignalTriggered::_copy()
{
  DataSignalTriggered *copy = new DataSignalTriggered(NULL);
  copy->_name = _name;
  copy->_module = _module;
  for(std::vector<DataParameter*>::iterator i = begin(); i != end(); i++)
  {
    copy->push_back((*i)->copy());
  }
  return copy;
}
