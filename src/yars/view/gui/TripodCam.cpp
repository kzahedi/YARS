#include "TripodCam.h"

TripodCam::TripodCam(WindowConfiguration *configuration)
  : FollowCamera(configuration, "Tripod camera")
{ }

void TripodCam::init(DataObject *f, DataCamera *cam)
{
  followable = f;
  _camera = cam;
}

void TripodCam::update()
{
  if(followable == NULL) return;
  updateFollowablePosition();
  _camera->setLookAt(followablePosition);
}
