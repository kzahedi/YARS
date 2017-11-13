#ifndef __ORBIT_CAM_H__
#define __ORBIT_CAM_H__

#include <yars/view/gui/FollowCamera.h>
#include <yars/util/PID.h>


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
    double _distance;
    double _angle;
    double _angularVelocity;
    double _zOffset;

    DataCamera *_camera;

    PID _lookAtX;
    PID _lookAtY;
    PID _lookAtZ;

    PID _fromX;
    PID _fromY;
    PID _fromZ;

    double x;
    double y;

};

#endif // __ORBIT_CAM_H__


