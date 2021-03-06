#include "OffsetCam.h"

#include <yars/configuration/data/Data.h>

OffsetCam::OffsetCam(WindowConfiguration *configuration)
  : FollowCamera(configuration, "Offset camera")
{ }

void OffsetCam::init(DataObject *f, DataCamera *cam)
{
  DataOffsetCam *data = Data::instance()->current()->screens()->cameraConfig()->offset();
  _camera = cam;
  followable = f;

  _lookAtX = data->lookAtX();
  _lookAtY = data->lookAtY();
  _lookAtZ = data->lookAtZ();

  _fromX = data->fromX();
  _fromY = data->fromY();
  _fromZ = data->fromZ();

  updateFollowablePosition();
  updatePositionOffset();

  distance = _camera->position() - followablePosition;

  _lookAtX.setInitialValue(followablePosition.x);
  _lookAtY.setInitialValue(followablePosition.y);
  _lookAtZ.setInitialValue(followablePosition.z);

  _fromX.setInitialValue(configuration->cameraPosition.x);
  _fromY.setInitialValue(configuration->cameraPosition.y);
  _fromZ.setInitialValue(configuration->cameraPosition.z);

}

void OffsetCam::update(P3D vel)
{
  if(followable == NULL) return;
  updateFollowablePosition();

  lookAtPosition.x = _lookAtX.update(followablePosition.x);
  lookAtPosition.y = _lookAtY.update(followablePosition.y);
  lookAtPosition.z = _lookAtZ.update(followablePosition.z);

  newCameraPosition.x = _fromX.update(followablePosition.x + distance.x + vel.x);
  newCameraPosition.y = _fromY.update(followablePosition.y + distance.y + vel.y);
  newCameraPosition.z = _fromZ.update(followablePosition.z + distance.z + vel.z);

  _camera->setPosition(newCameraPosition);
  _camera->setLookAt(lookAtPosition);
}
