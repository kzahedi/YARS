#ifndef __STATIC_FOLLOW_CAM_H__
#define __STATIC_FOLLOW_CAM_H__

#include <yars/view/gui/FollowCamera.h>
#include <yars/types/P3D.h>
#include <yars/util/PID.h>

class OffsetCam : public FollowCamera
{
  public:
    OffsetCam(WindowConfiguration *configuration);

    void init(DataObject* f, DataCamera *cam);
    void update(P3D vel);
    void name(string *name);

  private:
    P3D newCameraPosition;
    P3D lookAtPosition;
    P3D distance;

    PID _lookAtX;
    PID _lookAtY;
    PID _lookAtZ;

    PID _fromX;
    PID _fromY;
    PID _fromZ;

    DataCamera *_camera;

};

#endif // __STATIC_FOLLOW_CAM_H__


