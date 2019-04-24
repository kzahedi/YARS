#include "DataFixedActuator.h"

# define YARS_STRING_SOURCE          (char*)"source"
# define YARS_STRING_DESTINATION     (char*)"destination"
# define YARS_STRING_NAME            (char*)"name"
# define YARS_STRING_NAME_DEFINITION (char*)"name_definition"

DataFixedActuator::DataFixedActuator(DataNode *parent)
  : DataActuator(parent, DATA_ACTUATOR_FIXED)
{

}

void DataFixedActuator::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_FIXED))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_FIXED))
  {
    element->set(YARS_STRING_NAME, _name);
  }
  if(element->opening(YARS_STRING_SOURCE))
  {
    element->set(YARS_STRING_NAME, _source);
  }
  if(element->opening(YARS_STRING_DESTINATION))
  {
    element->set(YARS_STRING_NAME, _destination);
  }
}

string DataFixedActuator::name()
{
  return _name;
}

string DataFixedActuator::source()
{
  return _source;
}

string DataFixedActuator::destination()
{
  return _destination;
}


void DataFixedActuator::createXsd(XsdSpecification *spec)
{
  XsdSequence *fixedDefinition = new XsdSequence(YARS_STRING_FIXED_DEFINITION);
  fixedDefinition->add(NA(YARS_STRING_NAME,        YARS_STRING_XSD_STRING,      false));
  fixedDefinition->add(XE(YARS_STRING_SOURCE,      YARS_STRING_NAME_DEFINITION, 1, 1));
  fixedDefinition->add(XE(YARS_STRING_DESTINATION, YARS_STRING_NAME_DEFINITION, 0, 1));
  spec->add(fixedDefinition);
}


void DataFixedActuator::applyOffset(Pose pose)
{

}

DataFixedActuator* DataFixedActuator::_copy()
{
  DataFixedActuator *copy = new DataFixedActuator(NULL);
  copy->_name = _name;
  copy->_source = _source;
  copy->_destination = _destination;
  return copy;
}


