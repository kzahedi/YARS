#include "P3D.h"

P3D::P3D()
{
  x = 0;
  y = 0;
  z = 0;
}

P3D::P3D(const yReal* p)
{
  x = p[0];
  y = p[1];
  z = p[2];
}

P3D::P3D(const yReal _x, const yReal _y, const yReal _z)
{
  x = _x;
  y = _y;
  z = _z;
}

P3D::P3D(const P3D &p)
{
  x = p.x;
  y = p.y;
  z = p.z;
}

P3D & P3D::operator=(const P3D &b)
{
  x = b.x;
  y = b.y;
  z = b.z;
  return *this;
}

P3D & P3D::operator=(const yReal value)
{
  x = value;
  y = value;
  z = value;
  return *this;
}

P3D & P3D::operator=(const int value)
{
  *this = (yReal)value;
  return *this;
}

P3D & P3D::operator=(const yReal* values)
{
  x = values[0];
  y = values[1];
  z = values[2];
  return *this;
}


P3D & P3D::operator*=(const yReal value)
{
  x *= value;
  y *= value;
  z *= value;
  return *this;
}

P3D & P3D::operator/=(const yReal value)
{
  x /= value;
  y /= value;
  z /= value;
  return *this;
}

P3D & P3D::operator+=(const P3D &a)
{
  x += a.x;
  y += a.y;
  z += a.z;
  return *this;
}

P3D & P3D::operator+=(yReal value)
{
  x += value;
  y += value;
  z += value;
  return *this;
}

P3D & P3D::operator-=(const P3D &b)
{
  x = x - b.x;
  y = y - b.y;
  z = z - b.z;
  return *this;
}

const P3D P3D::operator+(const P3D &b) const
{
  P3D r = *this;
  r += b;
  return r;
}

const P3D P3D::operator-(const P3D &b) const
{
  P3D r = *this;
  r -= b;
  return r;
}

const P3D P3D::operator*(const P3D &b) const
{
  P3D r;
  r.x = y * b.z - z * b.y;
  r.y = z * b.x - x * b.z;
  r.z = x * b.y - y * b.x;
  return r;
}

yReal P3D::dot(const P3D &b) const
{
  yReal r = 0.0;
  r += x * b.x;
  r += y * b.y;
  r += z * b.z;
  return r;
}

const P3D P3D::operator*(const yReal value) const
{
  P3D r = *this;
  r *= value;
  return r;
}

void P3D::normalise()
{
  yReal l = sqrt(x * x + y * y + z * z);
  x /= l;
  y /= l;
  z /= l;
}

yReal P3D::length()
{
  return sqrt(x * x + y * y + z * z);
}

bool P3D::operator==(const P3D a)
{
  //return (x == a.x) && (y == a.y) && (z == a.z);
  return fabs(x - a.x) < 0.000001 && fabs(y - a.y) < 0.000001 && fabs(z - a.z) < 0.000001;
}

bool P3D::operator!=(const P3D a)
{
  //return (x == a.x) && (y == a.y) && (z == a.z);
  return fabs(x - a.x) > 0.000001 || fabs(y - a.y) > 0.000001 || fabs(z - a.z) > 0.000001;
}

void P3D::invert()
{
  x *= -1;
  y *= -1;
  z *= -1;
}

void P3D::rotate(P3D p)
{
  yReal sx = sin(p.x); yReal cx = cos(p.x);
  yReal sy = sin(p.y); yReal cy = cos(p.y);
  yReal sz = sin(p.z); yReal cz = cos(p.z);

  yReal n_x = x * cy * cz + z * sy - y * cy * sz;
  yReal n_y = -z * cy * sx + x * (cz * sx * sy + cx * sz) + y * (cx * cz - sx * sy * sz);
  yReal n_z = z * cx * cy + x * (-cx * cz * sy + sx * sz) + y * (cz * sx + cx * sy * sz);

  x = n_x;
  y = n_y;
  z = n_z;
}

void P3D::rotateInv(P3D p)
{
  yReal sx = sin(p.x); yReal cx = cos(p.x);
  yReal sy = sin(p.y); yReal cy = cos(p.y);
  yReal sz = sin(p.z); yReal cz = cos(p.z);

  yReal n_x = x * cy * cz + y * (cz * sx * sy - cx * sz) + z * (cx * cz * sy + sx * sz);
  yReal n_y = x * cy * sz + z * (-cz * sx + cx * sy * sz) + y * (cx * cz + sx * sy * sz);
  yReal n_z = z * cx * cy + y * cy * sx - x * sy;

  x = n_x;
  y = n_y;
  z = n_z;
}

yReal P3D::dist(P3D p)
{
  yReal x2 = (x - p.x) * (x - p.x);
  yReal y2 = (y - p.y) * (y - p.y);
  yReal z2 = (z - p.z) * (z - p.z);
  return sqrt(x2+y2+z2);
}
