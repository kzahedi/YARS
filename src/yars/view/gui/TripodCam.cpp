#include "TripodCam.h"

TripodCam::TripodCam(WindowConfiguration *configuration)
  : FollowCamera(configuration, "Tripod camera")
{ }

void TripodCam::init(DataObject *f, DataCamera *cam)
{
  followable = f;
  _camera = cam;
}

void TripodCam::update(P3D vel)
{
  if(followable == NULL) return;
  updateFollowablePosition();
  _camera->setLookAt(followablePosition);
}
