#include "DataSignalKeyboard.h"

# define YARS_STRING_MODULE (char*)"module"
# define YARS_STRING_KEY    (char*)"key"
# define YARS_STRING_NAME   (char*)"name"

DataSignalKeyboard::DataSignalKeyboard(DataNode *parent)
  : DataSignal(parent)
{

}

string DataSignalKeyboard::name()
{
  return _name;
}

string DataSignalKeyboard::module()
{
  return _module;
}

string DataSignalKeyboard::key()
{
  return _key;
}

void DataSignalKeyboard::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_KEYBOARD_SIGNAL))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_KEYBOARD_SIGNAL))
  {
    element->set(YARS_STRING_NAME,   _name);
    element->set(YARS_STRING_MODULE, _module);
    element->set(YARS_STRING_KEY,    _key);
  }
  if(element->opening(YARS_STRING_PARAMETER))
  {
    DataParameter *parameter = new DataParameter(this);
    parameter->add(element);
    current = parameter;
    push_back(parameter);
  }
}

void DataSignalKeyboard::createXsd(XsdSpecification *spec)
{
  XsdSequence *keyboardSignal = new XsdSequence(YARS_STRING_KEYBOARD_SIGNAL_DEFINITION);
  keyboardSignal->add(NA(YARS_STRING_NAME,      YARS_STRING_XSD_STRING,           true));
  keyboardSignal->add(NA(YARS_STRING_MODULE,    YARS_STRING_XSD_STRING,           true));
  keyboardSignal->add(NA(YARS_STRING_KEY,       YARS_STRING_XSD_STRING,           true));
  keyboardSignal->add(XE(YARS_STRING_PARAMETER, YARS_STRING_PARAMETER_DEFINITION, 0, 1));
  spec->add(keyboardSignal);
}

DataSignalKeyboard* DataSignalKeyboard::_copy()
{
  DataSignalKeyboard *copy = new DataSignalKeyboard(NULL);
  copy->_name = _name;
  copy->_module = _module;
  copy->_key = _key;
  for(std::vector<DataParameter*>::iterator i = begin(); i != end(); i++)
  {
    copy->push_back((*i)->copy());
  }
  return copy;
}
