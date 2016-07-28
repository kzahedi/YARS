#include "DataMacros.h"

DataMacros::DataMacros(DataNode *parent)
  : DataNode(parent)
{

}

DataMacros::~DataMacros()
{
  for(std::vector<DataMacro*>::iterator i = begin(); i != end(); i++)
  {
    delete *i;
  }
  clear();
}

DataMacro* DataMacros::macro(int index)
{
  return at(index);
}

DataMacro* DataMacros::macro(string name)
{
  for(std::vector<DataMacro*>::iterator i = begin(); i != end(); i++)
  {
    if((*i)->name() == name)
    {
      return (*i);
    }
  }
  return NULL;
}


void DataMacros::add(DataParseElement *element)
{
  if(element->opening(YARS_STRING_MACRO))
  {
    DataMacro *macro = new DataMacro(this);
    current = macro;
    macro->add(element);
    push_back(macro);
  }
  if(element->closing(YARS_STRING_MACROS))
  {
    current = parent;
  }
}

void DataMacros::createXsd(XsdSpecification *spec)
{
  XsdSequence *macrosConfigDefinition = new XsdSequence(YARS_STRING_MACROS_DEFINITION);
  macrosConfigDefinition->add(NE(YARS_STRING_MACRO, YARS_STRING_MACRO_DEFINITION, 1));
  spec->add(macrosConfigDefinition);
  DataMacro::createXsd(spec);
}


DataMacros* DataMacros::copy()
{
  DataMacros *copy = new DataMacros(NULL);
  for(std::vector<DataMacro*>::iterator i = begin(); i != end(); i++)
  {
    copy->push_back((*i)->copy());
  }
  return copy;
}
