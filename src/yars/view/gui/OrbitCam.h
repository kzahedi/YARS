#ifndef __ORBIT_CAM_H__
#define __ORBIT_CAM_H__

#include "util/PID.h"

#include "FollowCamera.h"

class OrbitCam : public FollowCamera
{
  public:
    OrbitCam(WindowConfiguration *configuration);

    void init(DataObject *f, DataCamera *cam);
    void update(P3D vel);
    void name(string *name);

  private:
    P3D newCameraPosition;
    P3D lookAtPosition;
    yReal _distance;
    yReal _angle;
    yReal _angularVelocity;
    yReal _zOffset;

    DataCamera *_camera;

    PID _lookAtX;
    PID _lookAtY;
    PID _lookAtZ;

    PID _fromX;
    PID _fromY;
    PID _fromZ;

    yReal x;
    yReal y;

};

#endif // __ORBIT_CAM_H__


