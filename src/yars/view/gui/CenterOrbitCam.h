#ifndef __CENTER_ORBIT_CAM_H__
#define __CENTER_ORBIT_CAM_H__

#include "FollowCamera.h"

class CenterOrbitCam : public FollowCamera
{
  public:
    CenterOrbitCam(WindowConfiguration *configuration);

    void init(DataObject* f, DataCamera *cam);
    void update(P3D vel);
    void name(string *name);

  private:
    P3D _newCameraPosition;
    P3D _center;
    double _distance;
    double _angle;
    double _angularVelocity;
    DataCamera *_camera;
};

#endif // __CENTER_ORBIT_CAM_H__


