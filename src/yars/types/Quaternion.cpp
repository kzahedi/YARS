#include "Quaternion.h"

Quaternion::Quaternion(yReal _w, yReal _x, yReal _y, yReal _z)
{
  w = _w;
  x = _x;
  y = _y;
  z = _z;
}

#ifndef _MSC_VER
Quaternion::Quaternion(Quaternion &q)
{
  *this = q;
}
#endif // _MSC_VER

Quaternion::Quaternion(const Quaternion &q)
{
  *this = q;
}

Quaternion::Quaternion(P3D p)
{
  *this << p;
}

Quaternion& Quaternion::operator=(const Quaternion &q)
{
  w = q.w;
  x = q.x;
  y = q.y;
  z = q.z;
  return *this;
}

Quaternion Quaternion::operator*(const Quaternion &q)
{
  Quaternion n(w, x, y, z);
  n *= q;
  return n;
}

Quaternion& Quaternion::operator*=(const Quaternion &q)
{
  // CHECKED 11.07.07

  yReal nw = w * q.w - x * q.x - y * q.y - z * q.z;
  yReal nx = w * q.x + x * q.w + y * q.z - z * q.y;
  yReal ny = w * q.y - x * q.z + y * q.w + z * q.x;
  yReal nz = w * q.z + x * q.y - y * q.x + z * q.w;

  w = nw;
  x = nx;
  y = ny;
  z = nz;

  return *this;
}

// euler to quaternion
Quaternion& Quaternion::operator<<(const P3D &p)
{
  // yReal c1   = cos(p.y*0.5);
  // yReal s1   = sin(p.y*0.5);
  // yReal c2   = cos(p.z*0.5);
  // yReal s2   = sin(p.z*0.5);
  // yReal c3   = cos(p.x*0.5);
  // yReal s3   = sin(p.x*0.5);
  // yReal c1c2 = c1*c2;
  // yReal s1s2 = s1*s2;
  // w = c1c2*c3  - s1s2*s3;
  // x = c1c2*s3  + s1s2*c3;
  // y = s1*c2*c3 + c1*s2*s3;
  // z = c1*s2*c3 - s1*c2*s3;

  yReal sx = sin(p.x/2); 
  yReal sy = sin(p.y/2);
  yReal sz = sin(p.z/2);
  yReal cx = cos(p.x/2);
  yReal cy = cos(p.y/2);
  yReal cz = cos(p.z/2);
  w = cx*cy*cz + sx*sy*sz;
  x = sx*cy*cz - cx*sy*sz;
  y = cx*sy*cz + sx*cy*sz;
  z = cx*cy*sz - sx*sy*cz;
  return *this;
}


Quaternion Quaternion::operator-() const
{
  Quaternion q(-w, -x, -y, -z);
  return q;
}

void Quaternion::conjugate()
{
  x *= -1;
  y *= -1;
  z *= -1;
}

void Quaternion::invert()
{
  double d = w*w + x*x + y*y + z*z;
  w =  w/d;
  x = -x/d;
  y = -y/d;
  z = -z/d;
}
