#include "CenterOrbitCam.h"

#include "util/macros.h"
#include "configuration/YarsConfiguration.h"

#include "configuration/data/Data.h"

CenterOrbitCam::CenterOrbitCam(WindowConfiguration *configuration)
  : FollowCamera(configuration, "CenterOrbit camera")
{
  _distance        = 0;
  _angle           = 0;
  _center          = 0.0;
}

void CenterOrbitCam::init(DataObject *f, DataCamera *cam)
{
  DataCenterOrbitCam *data = Data::instance()->current()->screens()->cameraConfig()->center();
  _camera = cam;
  _angularVelocity = data->speed() * 0.1 * M_PI / (yReal)__YARS_GET_SIMULATOR_FREQUENCY;
  P3D pos = _camera->position();
  _distance = distXY(_center, pos);
  yReal x = _center.x - pos.x;
  yReal y = _center.y - pos.y;
  _angle = atan2(x, y);
}

void CenterOrbitCam::update(P3D vel)
{
  _angle += _angularVelocity;
  P3D pos = _camera->position() + vel;
  _distance = distXY(_center, pos);
  _newCameraPosition.x = _center.x + sin(_angle) * _distance;
  _newCameraPosition.y = _center.y + cos(_angle) * _distance;
  _newCameraPosition.z = _camera->position().z;

  _camera->setLookAt(_center);
  _camera->setPosition(_newCameraPosition);
}
