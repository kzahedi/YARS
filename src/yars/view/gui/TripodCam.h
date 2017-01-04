#ifndef __TRIPOD_CAM_H__
#define __TRIPOD_CAM_H__

#include <yars/view/gui/FollowCamera.h>

class TripodCam : public FollowCamera
{
  public:
    TripodCam(WindowConfiguration *configuration);

    void init(DataObject* f, DataCamera *cam);
    void update(P3D vel);
    void name(string *name);

  private:
    P3D newCamPos;
    DataCamera *_camera;
};

#endif // __TRIPOD_CAM_H__


