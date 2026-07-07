#include <yars/configuration/data/DataLoggingSensor.h>

#include <yars/util/YarsErrorHandler.h>

#include <sstream>

using namespace std;

#define YARS_STRING_TARGET                    (char*)"target"
#define YARS_STRING_SENSOR_VALUE_DEFINITION   (char*)"value_definition"
#define YARS_STRING_SENSOR_USE_DEFINITION     (char*)"sensor_use_definition"
#define YARS_STRING_PRECISION                 (char*)"precision"
#define YARS_STRING_POSITIVE_NON_ZERO_INTEGER (char*)"positive_non_zero_integer_definition"
#define YARS_STRING_INTERNAL                  (char*)"internal"
#define YARS_STRING_EXTERNAL                  (char*)"external"
#define YARS_STRING_EMPTY_DEFINITION          (char*)"empty_definition"
#define YARS_NO_NAME                          (char*)""

#define YARS_INTERNAL_EXTERNAL_OPTION         (char*)"internal_external_option"
#define YARS_EXTERNAL_INTERNAL_OPTION         (char*)"external_internal_option"


DataLoggingSensor::DataLoggingSensor(DataNode *parent)
  : DataNode(parent)
{
  _sensor = NULL;
  _precision = -1;
}

DataLoggingSensor::~DataLoggingSensor()
{
  if(_sensor != NULL) delete _sensor;
  clear();
}

void DataLoggingSensor::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_LOGGING_SENSOR))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_LOGGING_SENSOR))
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
}

DataLoggingSensor* DataLoggingSensor::copy()
{
  DataLoggingSensor *copy = new DataLoggingSensor(NULL);
  copy->_target = _target;
  for (const auto &v : *this)   copy->push_back(v);
  for (int v : _order)          copy->_order.push_back(v);
  copy->_precision = _precision;
  return copy;
}

string DataLoggingSensor::target()
{
  return _target;
}

DataSensor* DataLoggingSensor::sensor()
{
  return _sensor;
}

void DataLoggingSensor::set(DataSensor* sensor)
{
  _sensor = sensor;
}

int DataLoggingSensor::precision()
{
  return _precision;
}

std::vector<int> DataLoggingSensor::order()
{
  return _order;
}
