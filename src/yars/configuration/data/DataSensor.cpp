#include "DataSensor.h"

DataSensor::DataSensor(DataNode *parent, int type)
  : DataNode(parent)
{
  _noise  = NULL;
  _filter = NULL;
  _type   = type;
}

DataSensor::~DataSensor()
{
  if(_noise  != NULL) delete _noise;
  if(_filter != NULL) delete _filter;
}

int DataSensor::type()
{
  return _type;
}

void DataSensor::applyOffset(Pose)
{
  // not all sensors have a pose. that is why this is not a virtual function
}

DataSensor* DataSensor::copy()
{
  DataSensor *copy = _copy();
  copy->_type = _type;
  return copy;
}

void DataSensor::resetTo(const DataSensor* other)
{
  _resetTo(other);
}

string DataSensor::name()
{
  return _name;
}

DataNoise* DataSensor::noise()
{
  return _noise;
}

DataFilter* DataSensor::filter()
{
  return _filter;
}

Domain DataSensor::mapping()
{
  return _mapping;
}

string DataSensor::object()
{
  return _object;
}

string DataSensor::logName(int index)
{
  return name();
}
