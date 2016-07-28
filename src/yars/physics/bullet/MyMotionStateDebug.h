#ifndef __MOTION_STATE_DEBUG_H
#define __MOTION_STATE_DEBUG_H

#include <yars/configuration/data/DataObject.h>

#include <btBulletDynamicsCommon.h>

class MyMotionStateDebug : public btMotionState
{
  public:
    MyMotionStateDebug(DataObject *data);

    void getWorldTransform(btTransform &worldTrans) const;
    void setWorldTransform(const btTransform &worldTrans);

  private:
    DataObject *_data;
    btTransform _transform;

};

#endif // __MOTION_STATE_DEBUG_H
