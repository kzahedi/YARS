#ifndef __COMPOUND_MOTION_STATE_H__
#define __COMPOUND_MOTION_STATE_H__

#include <yars/configuration/data/DataComposite.h>
#include <yars/configuration/data/DataObjects.h>

#include <btBulletDynamicsCommon.h>

class MyCompositeMotionState : public btMotionState
{
  public:
    MyCompositeMotionState(DataComposite *data, btTransform transform);

    void getWorldTransform(btTransform &worldTrans) const;
    void setWorldTransform(const btTransform &worldTrans);
    void addObject(DataObject *object);
    void setShape(btCompoundShape *shape);

  private:
    DataComposite    *_data;
    btTransform      _transform;
    DataObjects      _objects;
    btCompoundShape *_shape;

};

#endif // __COMPOUND_MOTION_STATE_H__

