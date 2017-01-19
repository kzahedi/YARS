#include <yars/configuration/data/DataTrace.h>

#include <yars/configuration/YarsConfiguration.h>

#include <iostream>

using namespace std;

DataTrace::DataTrace()
{
  set_capacity(10);
  clear();
  _useLocal = true;
  _target = NULL;
}

void DataTrace::update()
{
  if(_useLocal)
  {
    P3D pos      = _target->pose().position;
    Quaternion q = _target->quaternion();
    P3D o        = _offset;
    if(_project)
    {
      switch(_projectionPlane)
      {
        case DATA_TRACE_PROJECT_XY:
          pos.z = 0.0;
          break;
        case DATA_TRACE_PROJECT_XZ:
          pos.y = 0.0;
          break;
        case DATA_TRACE_PROJECT_YZ:
          pos.x = 0.0;
          break;
      }
    }
    else
    {
      o         *= q;
    }
    pos         += o;
    push_back(pos);
  }
  else
  {
    P3D pos = _target->pose().position;
    if(_project)
    {
      switch(_projectionPlane)
      {
        case DATA_TRACE_PROJECT_XY:
          pos.z = 0.0;
          break;
        case DATA_TRACE_PROJECT_XZ:
          pos.y = 0.0;
          break;
        case DATA_TRACE_PROJECT_YZ:
          pos.x = 0.0;
          break;
      }
    }
    pos += _offset;
    push_back(pos);
  }
}

void DataTrace::setTarget(DataObject *t)
{
  _target = t;
}

void DataTrace::useLocalCoordinateSystem(bool useLocal)
{
  _useLocal = useLocal;
}

int DataTrace::step()
{
  return _step;
}

int DataTrace::maximum()
{
  return _max;
}

P3D DataTrace::offset()
{
  return _offset;
}

Colour DataTrace::color()
{
  return _color;
}

void DataTrace::reset()
{
  clear();
}
