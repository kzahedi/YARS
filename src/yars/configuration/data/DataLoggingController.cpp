#include <yars/configuration/data/DataLoggingController.h>

#include <yars/util/stl_macros.h>
#include <yars/util/YarsErrorHandler.h>

#include <sstream>

using namespace std;

#define YARS_STRING_TARGET                    (char*)"target"
#define YARS_STRING_CONTROLLER_USE_DEFINITION (char*)"controller_use_definition"
#define YARS_STRING_PRECISION                 (char*)"precision"
#define YARS_STRING_POSITIVE_NON_ZERO_INTEGER (char*)"positive_non_zero_integer_definition"
#define YARS_STRING_USE                       (char*)"use"
#define YARS_NO_NAME                          (char*)""
#define YARS_STRING_CONTROLLER_USE_DEFINITION (char*)"controller_use_definition"

#define YARS_INTERNAL_EXTERNAL_OPTION         (char*)"internal_external_option"
#define YARS_EXTERNAL_INTERNAL_OPTION         (char*)"external_internal_option"


DataLoggingController::DataLoggingController(DataNode *parent)
  : DataNode(parent)
{
  _controller = NULL;
  _precision = -1;
}

DataLoggingController::~DataLoggingController()
{
  if(_controller != NULL) delete _controller;
  clear();
}

void DataLoggingController::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_LOGGING_CONTROLLER))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_LOGGING_CONTROLLER))
  {
    element->set(YARS_STRING_TARGET, _target);
    element->set(YARS_STRING_PRECISION, _precision);
  }
  if(element->opening(YARS_STRING_USE))
  {
    string v;
    element->set(YARS_STRING_VALUE, v);
    push_back(v);
  }
}

void DataLoggingController::createXsd(XsdSpecification *spec)
{
  XsdSequence *loggingControllerDefinition = new XsdSequence(YARS_STRING_LOGGING_CONTROLLER_DEFINITION);
  loggingControllerDefinition->add(NA(YARS_STRING_PRECISION, YARS_STRING_POSITIVE_NON_ZERO_INTEGER, false));
  loggingControllerDefinition->add(NA(YARS_STRING_TARGET,    YARS_STRING_XSD_STRING,                true));
  loggingControllerDefinition->add(NE(YARS_STRING_USE,       YARS_STRING_CONTROLLER_USE_DEFINITION, true));
  spec->add(loggingControllerDefinition);

  XsdSequence *useDefinition = new XsdSequence(YARS_STRING_CONTROLLER_USE_DEFINITION);
  useDefinition->add(NA(YARS_STRING_VALUE, YARS_STRING_XSD_STRING, true));
  spec->add(useDefinition);
}

DataLoggingController* DataLoggingController::copy()
{
  DataLoggingController *copy = new DataLoggingController(NULL);
  copy->_target = _target;
  for(std::vector<string>::iterator v = begin(); v != end(); v++) copy->push_back(*v);
  FOREACH(int, v, _order) copy->_order.push_back(*v);
  copy->_precision = _precision;
  return copy;
}

string DataLoggingController::target()
{
  return _target;
}

DataController* DataLoggingController::controller()
{
  return _controller;
}

void DataLoggingController::set(DataController* controller)
{
  _controller = controller;
}

int DataLoggingController::precision()
{
  return _precision;
}

std::vector<int> DataLoggingController::order()
{
  return _order;
}

