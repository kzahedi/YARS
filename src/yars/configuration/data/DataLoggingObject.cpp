#include <yars/configuration/data/DataLoggingObject.h>


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
