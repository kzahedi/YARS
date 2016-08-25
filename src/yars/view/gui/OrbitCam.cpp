#include "OrbitCam.h"

#include "util/macros.h"
#include "configuration/YarsConfiguration.h"
#include <yars/util/macros.h>
#include <math.h>

#include "configuration/data/Data.h"

OrbitCam::OrbitCam(WindowConfiguration *configuration)
  : FollowCamera(configuration, "Orbit camera")
{
  _distance = 0;
  _angle    = 0;
}

void OrbitCam::init(DataObject *f, DataCamera *cam)
{
  DataOrbitCam *config = Data::instance()->current()->screens()->cameraConfig()->orbit();
  _camera = cam;
  P3D pos = _camera->position();

  _angularVelocity = config->speed();
  _angularVelocity = _angularVelocity * 0.1 * M_PI / (yReal)__YARS_GET_SIMULATOR_FREQUENCY;

  _lookAtX = config->lookAtX();
  _lookAtY = config->lookAtY();
  _lookAtZ = config->lookAtZ();

  _fromX = config->fromX();
  _fromY = config->fromY();
  _fromZ = config->fromZ();

  followable = f;
  updateFollowablePosition();
  _distance = distXY(_camera->position(), followablePosition);
  x = followablePosition.x - pos.x;
  y = followablePosition.y - pos.y;
  _zOffset   = pos.z - followablePosition.z ;
  _angle = atan2(x, y) + M_PI;

  _lookAtX.setInitialValue(followablePosition.x);
  _lookAtY.setInitialValue(followablePosition.y);
  _lookAtZ.setInitialValue(followablePosition.z);

  _fromX.setInitialValue(pos.x);
  _fromY.setInitialValue(pos.y);
  _fromZ.setInitialValue(pos.z);
}

void OrbitCam::update(P3D vel)
{
  updateFollowablePosition();

  if(vel.length() > 0.01)
  {
    P3D pos = _camera->position() + vel;
    _distance = distXY(pos, followablePosition);
    newCameraPosition.x = pos.x;
    newCameraPosition.y = pos.y;
    newCameraPosition.z = pos.z;
    _fromX.reset();
    _fromY.reset();
    _fromZ.reset();
    _fromX.setInitialValue(pos.x);
    _fromY.setInitialValue(pos.y);
    _fromZ.setInitialValue(pos.z);
    x = followablePosition.x - pos.x;
    y = followablePosition.y - pos.y;
    _zOffset   = pos.z - followablePosition.z ;
    _angle = atan2(x, y) + M_PI;
  }
  else
  {
    _angle += _angularVelocity;

    newCameraPosition.x = _fromX.update(followablePosition.x + sin(_angle) * _distance);
    newCameraPosition.y = _fromY.update(followablePosition.y + cos(_angle) * _distance);
    newCameraPosition.z = _fromZ.update(followablePosition.z + _zOffset);
  }

  lookAtPosition.x = _lookAtX.update(followablePosition.x);
  lookAtPosition.y = _lookAtY.update(followablePosition.y);
  lookAtPosition.z = _lookAtZ.update(followablePosition.z);

  _camera->setPosition(newCameraPosition);
  _camera->setLookAt(lookAtPosition);
  __YARS_SET_CAMERA_POSITION(newCameraPosition);
}
