#include <yars/configuration/data/DataLoggingActuator.h>
#include "DataBinding.h"

#include <yars/util/YarsErrorHandler.h>

#include <cstdlib>
#include <sstream>

using namespace std;

#define YARS_STRING_TARGET                    (char*)"target"
#define YARS_STRING_ACTUATOR_VALUE_DEFINITION (char*)"value_definition"
#define YARS_STRING_PRECISION                 (char*)"precision"
#define YARS_STRING_INTERNAL                  (char*)"internal"
#define YARS_STRING_EXTERNAL                  (char*)"external"
#define YARS_STRING_DESIRED                   (char*)"desired"
#define YARS_STRING_APPLIED_FORCE             (char*)"appliedForce"
#define YARS_STRING_APPLIED_VELOCITY          (char*)"appliedVelocity"
#define YARS_STRING_EMPTY                     (char*)"empty"
#define YARS_NO_NAME                          (char*)""
#define YARS_INTERNAL_EXTERNAL_OPTION         (char*)"internal_external_option"
#define YARS_EXTERNAL_INTERNAL_OPTION         (char*)"external_internal_option"

#define YARS_STRING_ACTUATOR_USE_DEFINITION   (char*)"actuator_use_definition"
#define YARS_STRING_POSITIVE_NON_ZERO_INTEGER (char*)"positive_non_zero_integer_definition"


namespace
{
// Attribute binding table for the actuator logger's own opening tag.
// Child-element dispatch (internal/external/desired/appliedForce/
// appliedVelocity) stays hand-written below.
const std::vector<yars::AttributeBinding> &loggingActuatorAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_TARGET,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingActuator *>(self)->setTarget(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_PRECISION,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingActuator *>(self)->setPrecision(atoi(value.c_str())); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

DataLoggingActuator::DataLoggingActuator(DataNode *parent)
  : DataNode(parent)
{
  _actuator = NULL;
  _precision = -1;
}

DataLoggingActuator::~DataLoggingActuator()
{
  if(_actuator != NULL) delete _actuator;
  clear();
}

void DataLoggingActuator::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_LOGGING_ACTUATOR))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_LOGGING_ACTUATOR))
  {
    yars::applyAttributes(this, element, loggingActuatorAttributeBindings());
  }
  if(element->opening(YARS_STRING_INTERNAL))
  {
    _order.push_back(INTERNAL); // internal
  }
  if(element->opening(YARS_STRING_EXTERNAL))
  {
    _order.push_back(EXTERNAL); // external
  }
  if(element->opening(YARS_STRING_DESIRED))
  {
    _order.push_back(DESIRED); // desired
  }
  if(element->opening(YARS_STRING_APPLIED_FORCE))
  {
    _order.push_back(APPLIED_FORCE); // external
  }
  if(element->opening(YARS_STRING_APPLIED_VELOCITY))
  {
    _order.push_back(APPLIED_VELOCITY); // external
  }
}

DataLoggingActuator* DataLoggingActuator::copy()
{
  DataLoggingActuator *copy = new DataLoggingActuator(NULL);
  copy->_target = _target;
  for (const auto &v : *this)   copy->push_back(v);
  for (int v : _order)          copy->_order.push_back(v);
  copy->_precision = _precision;
  return copy;
}

string DataLoggingActuator::target()
{
  return _target;
}

void DataLoggingActuator::setTarget(string target)
{
  _target = target;
}

void DataLoggingActuator::setPrecision(int precision)
{
  _precision = precision;
}

DataActuator* DataLoggingActuator::actuator()
{
  return _actuator;
}

void DataLoggingActuator::set(DataActuator* actuator)
{
  _actuator = actuator;
}

int DataLoggingActuator::precision()
{
  return _precision;
}

std::vector<int> DataLoggingActuator::order()
{
  return _order;
}

