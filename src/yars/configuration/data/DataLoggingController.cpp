#include <yars/configuration/data/DataLoggingController.h>
#include "DataBinding.h"

#include <yars/util/YarsErrorHandler.h>

#include <cstdlib>
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


namespace
{
// Attribute binding table for the controller logger's own opening tag.
// Child-element dispatch (use) stays hand-written below.
const std::vector<yars::AttributeBinding> &loggingControllerAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_TARGET,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingController *>(self)->setTarget(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_PRECISION,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingController *>(self)->setPrecision(atoi(value.c_str())); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

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
    yars::applyAttributes(this, element, loggingControllerAttributeBindings());
  }
  if(element->opening(YARS_STRING_USE))
  {
    string v;
    element->set(YARS_STRING_VALUE, v);
    push_back(v);
  }
}

DataLoggingController* DataLoggingController::copy()
{
  DataLoggingController *copy = new DataLoggingController(NULL);
  copy->_target = _target;
  for (const auto &v : *this)   copy->push_back(v);
  for (int v : _order)          copy->_order.push_back(v);
  copy->_precision = _precision;
  return copy;
}

string DataLoggingController::target()
{
  return _target;
}

void DataLoggingController::setTarget(string target)
{
  _target = target;
}

void DataLoggingController::setPrecision(int precision)
{
  _precision = precision;
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

