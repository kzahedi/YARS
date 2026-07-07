#include "DataFilter.h"

# define YARS_STRING_MODULE (char*)"module"
# define YARS_STRING_NAME   (char*)"name"

DataFilter::DataFilter(DataNode *parent)
  : DataNode(parent)
{

}

DataFilter::~DataFilter()
{
  for(std::vector<DataParameter*>::iterator i = begin(); i != end(); i++)
  {
    delete (*i);
  }
  clear();
}

void DataFilter::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_FILTER))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_FILTER))
  {
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

string DataFilter::module()
{
  return _module;
}

DataParameter* DataFilter::parameter(int index)
{
  return at(index);
}

DataFilter* DataFilter::copy()
{
  DataFilter *copy = new DataFilter(NULL);
  copy->_module = _module;
  for(std::vector<DataParameter*>::iterator i = begin(); i != end(); i++)
  {
    copy->push_back((*i)->copy());
  }
  return copy;
}
