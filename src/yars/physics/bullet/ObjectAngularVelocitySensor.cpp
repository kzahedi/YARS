#include <yars/physics/bullet/ObjectAngularVelocitySensor.h>

#include <yars/types/Quaternion.h>

ObjectAngularVelocitySensor::ObjectAngularVelocitySensor(DataObjectAngularVelocitySensor *data, Robot *robot)
  : Sensor(data->name(), data->object(), robot)
{
  _data = data;
}

ObjectAngularVelocitySensor::~ObjectAngularVelocitySensor()
{
}

void ObjectAngularVelocitySensor::prePhysicsUpdate()
{
}

void ObjectAngularVelocitySensor::postPhysicsUpdate()
{
  // cout << "ObjectAngularVelocitySensor postPhysicsUpdate global = " << _data->global() << endl;
  P3D ang = _targetObject->data()->getCurrentAngularVelocity();
  int index = 0;
  if(_data->global())
  {
    if(_data->x()) _data->setInternalValue(index++,  ang.x);
    if(_data->y()) _data->setInternalValue(index++,  ang.y);
    if(_data->z()) _data->setInternalValue(index++,  ang.z);
  }
  else
  {
    Quaternion q(_targetObject->data()->pose().orientation);
    q.invert();
    ang *= q;
    if(_data->x()) _data->setInternalValue(index++,  ang.x);
    if(_data->y()) _data->setInternalValue(index++,  ang.y);
    if(_data->z()) _data->setInternalValue(index++,  ang.z);
  }
}
