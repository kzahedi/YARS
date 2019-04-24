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

void DataFilter::createXsd(XsdSpecification *spec)
{
  XsdSequence *filter = new XsdSequence(YARS_STRING_FILTER_DEFINITION);
  filter->add(NA(YARS_STRING_MODULE,    YARS_STRING_XSD_STRING,           true));
  filter->add(XE(YARS_STRING_PARAMETER, YARS_STRING_PARAMETER_DEFINITION, 0));
  spec->add(filter);

  XsdSequence *parameterDefinition = new XsdSequence(YARS_STRING_PARAMETER_DEFINITION);
  parameterDefinition->add(NA(YARS_STRING_NAME,  YARS_STRING_XSD_STRING,  true));
  parameterDefinition->add(NA(YARS_STRING_VALUE, YARS_STRING_XSD_STRING, true));
  spec->add(parameterDefinition);
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
