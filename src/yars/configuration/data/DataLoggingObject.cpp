#include <yars/configuration/data/DataLoggingObject.h>

#include <yars/util/stl_macros.h>

#include <yars/util/YarsErrorHandler.h>

#include <sstream>

using namespace std;

# define YARS_STRING_TARGET                    (char*)"target"
# define YARS_STRING_USE                       (char*)"use"
# define YARS_STRING_OBJECT_VALUE_DEFINITION   (char*)"value_definition"
# define YARS_STRING_OBJECT_USE_DEFINITION     (char*)"object_use_definition"
# define YARS_STRING_PRECISION                 (char*)"precision"
# define YARS_STRING_POSITIVE_NON_ZERO_INTEGER (char*)"positive_non_zero_integer_definition"

# define YARS_STRING_X                         "x"
# define YARS_STRING_Y                         "y"
# define YARS_STRING_Z                         "z"
# define YARS_STRING_ALPHA                     "alpha"
# define YARS_STRING_BETA                      "beta"
# define YARS_STRING_GAMMA                     "gamma"

DataLoggingObject::DataLoggingObject(DataNode *parent)
  : DataNode(parent)
{
  _object = NULL;
  _precision = -1;
}

DataLoggingObject::~DataLoggingObject()
{
  if(_object != NULL) delete _object;
  clear();
}

void DataLoggingObject::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_LOGGING_OBJECT))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_LOGGING_OBJECT))
  {
    element->set(YARS_STRING_TARGET,    _target);
    element->set(YARS_STRING_PRECISION, _precision);
  }
  if(element->opening(YARS_STRING_USE))
  {
    string v;
    element->set(YARS_STRING_VALUE, v);
    push_back(v);
  }
}

void DataLoggingObject::createXsd(XsdSpecification *spec)
{
  XsdSequence *loggingObjectDefinition = new XsdSequence(YARS_STRING_LOGGING_OBJECT_DEFINITION);
  loggingObjectDefinition->add(NA(YARS_STRING_TARGET, YARS_STRING_XSD_STRING, true));
  loggingObjectDefinition->add(NA(YARS_STRING_PRECISION, YARS_STRING_POSITIVE_NON_ZERO_INTEGER, false));
  loggingObjectDefinition->add(NE(YARS_STRING_USE, YARS_STRING_OBJECT_USE_DEFINITION, "0", YARS_STRING_XSD_UNBOUNDED));
  spec->add(loggingObjectDefinition);

  XsdSequence *useDefinition = new XsdSequence(YARS_STRING_OBJECT_USE_DEFINITION);
  useDefinition->add(NA(YARS_STRING_VALUE, YARS_STRING_OBJECT_VALUE_DEFINITION, true));
  spec->add(useDefinition);

  XsdEnumeration *objectUseEnum = new XsdEnumeration(YARS_STRING_OBJECT_VALUE_DEFINITION,
      YARS_STRING_XSD_STRING);
  objectUseEnum->add(YARS_STRING_X);
  objectUseEnum->add(YARS_STRING_Y);
  objectUseEnum->add(YARS_STRING_Z);
  objectUseEnum->add(YARS_STRING_ALPHA);
  objectUseEnum->add(YARS_STRING_BETA);
  objectUseEnum->add(YARS_STRING_GAMMA);
  spec->add(objectUseEnum);

}

DataLoggingObject* DataLoggingObject::copy()
{
  DataLoggingObject *copy = new DataLoggingObject(NULL);
  copy->_target = _target;
  copy->_precision = _precision;
  for(std::vector<string>::iterator v = begin(); v != end(); v++)
  {
    copy->push_back(*v);
  }
  return copy;
}

string DataLoggingObject::target()
{
  return _target;
}

DataObject* DataLoggingObject::object()
{
  return _object;
}

void DataLoggingObject::set(DataObject* object)
{
  _object = object;
}

int DataLoggingObject::precision()
{
  return _precision;
}
