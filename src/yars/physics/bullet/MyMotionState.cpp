#include <yars/physics/bullet/MyMotionState.h>
#include <yars/types/P3D.h>

MyMotionState::MyMotionState(DataObject *data)
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

void MyMotionState::getWorldTransform(btTransform &worldTrans) const
{
  worldTrans = _transform;
}

void MyMotionState::setWorldTransform(const btTransform &worldTrans)
{
  btQuaternion q = worldTrans.getRotation();
  btVector3    p = worldTrans.getOrigin();
  _data->setPose(Pose(p.getX(), p.getY(), p.getZ(), q.getX(), q.getY(), q.getZ(), q.getW()));
}
