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
  delete[] _rayPose;
  delete[] _end;
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
  const double fullLength = _data->distance();
  double length = fullLength;
  for (int i = 0; i < 5; i++)
  {
    const P3D &start = _rayCoordinates[i].pose.position;
    const P3D &endP  = _rayCoordinates[i].end;
    btVector3 bStart(start.x, start.y, start.z);
    btVector3 bEnd(endP.x, endP.y, endP.z);
    btScalar maxFraction = btScalar(1.0);
    if (length < fullLength)
      maxFraction = btScalar(length / fullLength + 1e-9);
    btVector3 bHit;
    const bool hitSomething = World::rayTest(bStart, bEnd, bHit, maxFraction);
    // Hit: identical value the unpruned query would produce.
    // Miss or pruned-beyond-threshold: replicate the ORIGINAL miss
    // arithmetic exactly — distance computed from the double-precision
    // `end`, which may update the min by an ulp, as the old code did.
    P3D hit = hitSomething ? P3D(bHit[0], bHit[1], bHit[2]) : endP;
    P3D diff = hit - _rayCoordinates[i].pose.position;
    double distance = diff.length();
    if (distance < length) length = distance;
  }
  _data->setMeasuredDistance(length);
  if (length < _data->distance())
    _data->setInternalValue(0, length);
  else
    _data->setInternalValue(0, _data->distance());
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
