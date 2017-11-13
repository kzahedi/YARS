#include "GenericProximitySensor.h"

#include <sstream>

GenericProximitySensor::GenericProximitySensor(DataGenericProximitySensor *data, Robot *robot)
 : Sensor(data->name(), data->object(), robot)
{
  _data    = data;
  _rayPose = new Pose[5];
  _end     = new P3D[5];
  __createRays();
}

GenericProximitySensor::~GenericProximitySensor()
{
  delete _rayPose;
  delete _end;
}

void GenericProximitySensor::prePhysicsUpdate()
{
  Pose objectPose = _targetObject->data()->pose();
  for(int i = 0; i < 5; i++)
  {
    _rayCoordinates[i].pose = _rayCoordinates[i].sensorPose;
    _rayCoordinates[i].pose << objectPose;

    P3D ray(0, 0, _data->distance());
    Quaternion q = _rayCoordinates[i].q;
    Quaternion r(_rayCoordinates[i].pose.orientation);

    q *= r;
    ray *= q;

    _rayCoordinates[i].end = _rayCoordinates[i].pose.position + ray;
  }
}

void GenericProximitySensor::postPhysicsUpdate()
{
  double length = _data->distance();
  for(int i = 0; i < 5; i++)
  {
    P3D hit = World::rayTest(_rayCoordinates[i].pose.position, _rayCoordinates[i].end);
    P3D diff = hit - _rayCoordinates[i].pose.position;
    double distance = diff.length();
    if(distance < length) length = distance;
  }
  _data->setMeasuredDistance(length);
  if(length < _data->distance())
  {
    _data->setInternalValue(0, length);
  }
  else
  {
    _data->setInternalValue(0, _data->distance());
  }
}

void GenericProximitySensor::__createRays()
{
  Pose objectPose = _targetObject->data()->pose();
  Pose sensorPose = _data->pose();

  std::vector<Quaternion> qs;

  qs.push_back(Quaternion(P3D( 0, 0,                         0)));
  qs.push_back(Quaternion(P3D( 0, 0,                         _data->openingAngles().x)));
  qs.push_back(Quaternion(P3D( 0, _data->openingAngles().y,  0)));
  qs.push_back(Quaternion(P3D( 0, 0,                         -_data->openingAngles().x)));
  qs.push_back(Quaternion(P3D( 0, -_data->openingAngles().y, 0)));

  for(int i = 0; i < 5; i++)
  {
    RayCoordinates rc;
    rc.q = qs[i];
    P3D r(0, 0, 1);
    r *= rc.q;
    rc.sensorPose = sensorPose;
    rc.pose = rc.sensorPose;
    rc.pose << objectPose;
    rc.end = rc.pose.position + r;
    _rayCoordinates.push_back(rc);
  }
}
