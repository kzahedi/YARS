#include <yars/physics/bullet/MyCompositeMotionState.h>

#include <yars/types/P3D.h>

MyCompositeMotionState::MyCompositeMotionState(DataComposite *data, btTransform transform)
{
  _data = data;
  _transform = transform;
}

void MyCompositeMotionState::getWorldTransform(btTransform &worldTrans) const
{
  worldTrans = _transform;
}

void MyCompositeMotionState::setWorldTransform(const btTransform &worldTrans)
{
  btQuaternion q = worldTrans.getRotation();
  btVector3    p = worldTrans.getOrigin();
  btTransform oTransform;
  _data->setPose(Pose(p.getX(), p.getY(), p.getZ(), q.getX(), q.getY(), q.getZ(), q.getW()));
  for(int i = 0; i < _objects.size(); i++)
  {
    oTransform    = worldTrans * _shape->getChildTransform(i);
    q             = oTransform.getRotation();
    p             = oTransform.getOrigin();
    _objects[i]->setPose(Pose(p.getX(), p.getY(), p.getZ(), q.getX(), q.getY(), q.getZ(), q.getW()));
  }
}

void MyCompositeMotionState::addObject(DataObject *object)
{
  _objects.push_back(object);
}

void MyCompositeMotionState::setShape(btCompoundShape *shape)
{
  _shape = shape;
}
