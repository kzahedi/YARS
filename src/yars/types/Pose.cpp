#include <yars/types/Pose.h>

#include <yars/types/Quaternion.h>

#include <yars/util/macros.h>


Pose::Pose()
{ }

Pose::Pose(double x, double y, double z, double ox, double oy, double oz)
{
  position.x    = x;
  position.y    = y;
  position.z    = z;
  orientation.x = x;
  orientation.y = y;
  orientation.z = z;
  q << orientation;
}

Pose::Pose(double x, double y, double z, double qx, double qy, double qz, double qw)
{
  position.x = x;
  position.y = y;
  position.z = z;
  q.x        = qx;
  q.y        = qy;
  q.z        = qz;
  q.w        = qw;
  orientation << q;
}

P3D Pose::deg()
{
  P3D p;
  p = orientation;
  p /= M_PI;
  p *= 180.0;
  return p;
}

void Pose::toRad()
{
  orientation.x = orientation.x / 180.0 * M_PI;
  orientation.y = orientation.y / 180.0 * M_PI;
  orientation.z = orientation.z / 180.0 * M_PI;
}

Pose& Pose::operator+=(const Pose &p)
{
  position    += p.position;
  orientation += p.orientation;
  q           *= p.q;
  return *this;
}

void Pose::toDeg()
{
  orientation.x = orientation.x / M_PI * 180.0;
  orientation.y = orientation.y / M_PI * 180.0;
  orientation.z = orientation.z / M_PI * 180.0;
}

Pose & Pose::operator=(const Pose &b)
{
  orientation = b.orientation;
  position    = b.position;
  q           = b.q;
  return *this;
}

#include <iostream>
using namespace std;

void Pose::operator<<(const Pose &p)
{
  //Quaternion local(orientation);
  Quaternion other = p.q;
  Quaternion result = other * q;

  position *= p.q;
  position += p.position;
  orientation << result;
  q = result;
}

