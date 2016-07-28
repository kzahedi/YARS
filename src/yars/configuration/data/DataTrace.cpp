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
  // if(__YARS_GET_STEP % _step > 0)
  // {
  // return;
  // }
  if(_useLocal)
  {
    P3D pos      = _target->pose().position;
    Quaternion q = _target->quaternion();
    P3D o        = _offset;
    // cout << "***" << endl;
    // cout << q << endl << pos << endl << o << endl;
    o           *= q;
    pos         += o;
    // cout << o << endl << pos << endl;
    // cout << "***" << endl;
    push_back(pos);
  }
  else
  {
    P3D pos = _target->pose().position;
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
