#include <yars/physics/bullet/ObjectVelocitySensor.h>

#include <yars/types/Quaternion.h>

ObjectVelocitySensor::ObjectVelocitySensor(DataObjectVelocitySensor *data, Robot *robot)
  : Sensor(data->name(), data->object(), robot)
{
  _data = data;
}

ObjectVelocitySensor::~ObjectVelocitySensor()
{
}

void ObjectVelocitySensor::prePhysicsUpdate()
{
}

void ObjectVelocitySensor::postPhysicsUpdate()
{
  // cout << "ObjectVelocitySensor postPhysicsUpdate global = " << _data->global() << endl;
  P3D vel = _targetObject->data()->getCurrentVelocity();
  int index = 0;
  if(_data->global())
  {
    if(_data->x()) _data->setInternalValue(index++,  vel.x);
    if(_data->y()) _data->setInternalValue(index++,  vel.y);
    if(_data->z()) _data->setInternalValue(index++,  vel.z);
  }
  else
  {
    Quaternion q(_targetObject->data()->pose().orientation);
    q.invert();
    vel *= q;
    if(_data->x()) _data->setInternalValue(index++,  vel.x);
    if(_data->y()) _data->setInternalValue(index++,  vel.y);
    if(_data->z()) _data->setInternalValue(index++,  vel.z);
  }
}
