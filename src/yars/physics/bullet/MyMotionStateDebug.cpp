#include <yars/physics/bullet/MyMotionStateDebug.h>
#include <yars/types/P3D.h>

MyMotionStateDebug::MyMotionStateDebug(DataObject *data)
{
  _data = data;
  _transform.setRotation(
      btQuaternion(_data->quaternion().x,
                   _data->quaternion().y,
                   _data->quaternion().z,
                   _data->quaternion().w));
  _transform.setOrigin(
      btVector3(_data->pose().position.x,
                _data->pose().position.y,
                _data->pose().position.z));
}

void MyMotionStateDebug::getWorldTransform(btTransform &worldTrans) const
{
  worldTrans = _transform;
}

void MyMotionStateDebug::setWorldTransform(const btTransform &worldTrans)
{
  btQuaternion q = worldTrans.getRotation();
  btVector3    p = worldTrans.getOrigin();
  _data->setPose(Pose(p.getX(), p.getY(), p.getZ(), q.getX(), q.getY(), q.getZ(), q.getW()));
  cout << "MyMotionStateDebug: setting " << _data->pose() << endl;
}
