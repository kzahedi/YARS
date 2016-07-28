#include <yars/configuration/data/DataActuator.h>
#include <yars/defines/mutex.h>

DataActuator::DataActuator(DataNode *parent, int type)
: DataNode(parent)
{
  _type = type;

  YM_INIT;
}

DataActuator::~DataActuator()
{
  YM_CLOSE;
}

int DataActuator::type()
{
  return _type;
}

int DataActuator::controlType()
{
  return _controlType;
}

DataActuator* DataActuator::copy()
{
  DataActuator *copy = _copy();
  copy->_type = _type;
  copy->_controlType = _controlType;
  return copy;
}

void DataActuator::setCurrentAxisPosition(P3D position)
{
  YM_LOCK;
  _axisPosition = position;
  YM_UNLOCK;
}

void DataActuator::setCurrentAxisOrientation(::Quaternion q)
{
  YM_LOCK;
  _axisOrientation = q;
  YM_UNLOCK;
}

P3D DataActuator::getCurrentAxisPosition()
{
  YM_LOCK;
  P3D r = _axisPosition;
  YM_UNLOCK;
  return r;
}

::Quaternion DataActuator::getCurrentAxisOrientation()
{
  YM_LOCK;
  Quaternion r = _axisOrientation;
  YM_UNLOCK;
  return r;
}

