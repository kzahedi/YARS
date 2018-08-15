#include <yars/configuration/data/DataLoggingActuator.h>

#include <yars/util/stl_macros.h>
#include <yars/util/YarsErrorHandler.h>

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
    element->set(YARS_STRING_TARGET, _target);
    element->set(YARS_STRING_PRECISION, _precision);
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

void DataLoggingActuator::createXsd(XsdSpecification *spec)
{
  XsdSequence *loggingActuatorDefinition = new XsdSequence(YARS_STRING_LOGGING_ACTUATOR_DEFINITION);
  loggingActuatorDefinition->add(NA(YARS_STRING_PRECISION, YARS_STRING_POSITIVE_NON_ZERO_INTEGER, false));
  loggingActuatorDefinition->add(NA(YARS_STRING_TARGET, YARS_STRING_XSD_STRING, true));
  spec->add(loggingActuatorDefinition);



  XsdChoice *options = new XsdChoice(YARS_NO_NAME, "0", YARS_STRING_XSD_UNBOUNDED);
  options->add(NE(YARS_STRING_INTERNAL,         YARS_STRING_EMPTY, 0,1));
  options->add(NE(YARS_STRING_EXTERNAL,         YARS_STRING_EMPTY, 0,1));
  options->add(NE(YARS_STRING_DESIRED,          YARS_STRING_EMPTY, 0,1));
  options->add(NE(YARS_STRING_APPLIED_FORCE,    YARS_STRING_EMPTY, 0,1));
  options->add(NE(YARS_STRING_APPLIED_VELOCITY, YARS_STRING_EMPTY, 0,1));
  loggingActuatorDefinition->add(options);

  XsdSequence *emptyDefinition = new XsdSequence(YARS_STRING_EMPTY);
  spec->add(emptyDefinition);
}

DataLoggingActuator* DataLoggingActuator::copy()
{
  DataLoggingActuator *copy = new DataLoggingActuator(NULL);
  copy->_target = _target;
  for(std::vector<string>::iterator v = begin(); v != end(); v++) copy->push_back(*v);
  FOREACH(int, v, _order) copy->_order.push_back(*v);
  copy->_precision = _precision;
  return copy;
}

string DataLoggingActuator::target()
{
  return _target;
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

