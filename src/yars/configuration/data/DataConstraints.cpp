#include "DataConstraints.h"

DataConstraints::DataConstraints(DataNode *parent)
  : DataNode(parent)
{

}

std::vector<DataConstraint*>::iterator DataConstraints::c_begin()
{
  return _constraints.begin();
}

std::vector<DataConstraint*>::iterator DataConstraints::c_end()
{
  return _constraints.end();
}

int DataConstraints::c_size()
{
  return (int)_constraints.size();
}

void DataConstraints::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_CONSTRAINTS))
  {
    current = parent;
  }
}

void DataConstraints::createXsd(XsdSpecification *spec)
{
}

