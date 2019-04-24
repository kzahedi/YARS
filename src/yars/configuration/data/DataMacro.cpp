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

void DataMacro::createXsd(XsdSpecification *spec)
{
  XsdSequence *macroDefinition = new XsdSequence(YARS_STRING_MACRO_DEFINITION);
  macroDefinition->add(NA(YARS_STRING_NAME, YARS_STRING_XSD_STRING, true));
  spec->add(macroDefinition);

  XsdChoice *objectChoice = new XsdChoice("", "1", YARS_STRING_XSD_UNBOUNDED);
  objectChoice->add(XE(YARS_STRING_OBJECT_BOX,             YARS_STRING_OBJECT_BOX_DEFINTION,             0));
  objectChoice->add(XE(YARS_STRING_OBJECT_SPHERE,          YARS_STRING_OBJECT_SPHERE_DEFINTION,          0));
  objectChoice->add(XE(YARS_STRING_OBJECT_PLY,             YARS_STRING_OBJECT_PLY_DEFINTION,             0));
  objectChoice->add(XE(YARS_STRING_OBJECT_CYLINDER,        YARS_STRING_OBJECT_CYLINDER_DEFINTION,        0));
  objectChoice->add(XE(YARS_STRING_OBJECT_CAPPED_CYLINDER, YARS_STRING_OBJECT_CAPPED_CYLINDER_DEFINTION, 0));
  objectChoice->add(XE(YARS_STRING_OBJECT_COMPOSITE,       YARS_STRING_OBJECT_COMPOSITE_DEFINITION,      0));
  macroDefinition->add(objectChoice);

  DataObjectFactory::createXsd(spec);
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
