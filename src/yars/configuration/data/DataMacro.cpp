#include "DataMacro.h"
#include "DataObjectFactory.h"

# define YARS_STRING_POSE                  (char*)"pose"
# define YARS_STRING_NAME                  (char*)"name"
# define YARS_STRING_BODY                  (char*)"body"
# define YARS_STRING_BODY_DEFINITION       (char*)"macro_body_definition"
# define YARS_STRING_SELF_COLLIDE          (char*)"selfCollide"
# define YARS_STRING_TRUE_FALSE_DEFINITION (char*)"true_false_definition"

DataMacro::DataMacro(DataNode *parent)
  : DataNode(parent)
{
}

DataMacro::~DataMacro()
{
  for(DataObjects::iterator i = begin(); i != end(); i++)
  {
    delete *i;
  }
  clear();
}

string DataMacro::name()
{
  return _name;
}

void DataMacro::add(DataParseElement *element)
{
  if(element->opening(YARS_STRING_MACRO))
  {
    element->set(YARS_STRING_NAME, _name);
  }

  if(element->closing(YARS_STRING_MACRO))
  {
    current = parent;
  }

  DataObject *object = DataObjectFactory::object(element, this);
  if(object != NULL)
  {
    current = object;
    push_back(object);
  }
}

DataMacro* DataMacro::copy()
{
  DataMacro *copy = new DataMacro(NULL);
  copy->_name = _name;
  for(DataObjects::iterator i = begin(); i != end(); i++)
  {
    copy->push_back((*i)->copy());
  }
  return copy;
}
