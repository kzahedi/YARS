#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include <yars/types/P3D.h>
#include <yars/defines/types.h>
#include <yars/defines/ymath.h>
#include <yars/util/macros.h>

#include <iostream>

using namespace std;

  class Quaternion
{
  public:
    Quaternion(yReal _w = 1, yReal _x = 0, yReal _y = 0, yReal _z = 0);
#ifndef _MSC_VER
    Quaternion(Quaternion &q);
#endif // _MSC_VER
    Quaternion(const Quaternion &q);
    Quaternion(P3D orientation);

    Quaternion& operator=(const Quaternion &q);
    Quaternion  operator*(const Quaternion &q);
    Quaternion& operator*=(const Quaternion &q);

    Quaternion& operator<<(const P3D &p); // euler to quaternion
    Quaternion operator-() const;
    void conjugate();
    void invert();

    friend P3D& operator*=(P3D& p, const Quaternion& q) // rotate vector by Quaternion
    {
      Quaternion v(0, p.x, p.y, p.z);
      Quaternion c(q);
      c.conjugate();
      Quaternion a(q);
      a *= v;
      a *= c;
      p.x = a.x;
      p.y = a.y;
      p.z = a.z;
      return p;
    }

    friend P3D& operator<<(P3D& p, const Quaternion& q) // quaternion to euler
    {
      double sqw;
      double sqx;
      double sqy;
      double sqz;

      sqw = q.w * q.w;
      sqx = q.x * q.x;
      sqy = q.y * q.y;
      sqz = q.z * q.z;

      p.x = (double)atan2l(2.0 * ( q.y * q.z + q.x * q.w ) , ( -sqx - sqy + sqz + sqw ));
      // cout << q << " : " << -2.0 * ( q.x * q.z - q.y * q.w ) << endl;
      if(fabs(-2.0 * ( q.x * q.z - q.y * q.w ) - 1.0) < 0.000001)
      {
        p.y = M_PI_2;
      }
      else
      {
        p.y = (double)asinl(-2.0 * ( q.x * q.z - q.y * q.w ));
      }
      p.z = (double)atan2l(2.0 * ( q.x * q.y + q.z * q.w ) , (  sqx - sqy - sqz + sqw ));
      // cout << "P3D operator<< " << p << endl;

      return p;
    }

    friend std::ostream& operator<<(std::ostream& str, const Quaternion& q)
    {
      str << q.w << ", " << q.x << ", " << q.y << ", " << q.z;
      return str;
    };

    yReal w;
    yReal x;
    yReal y;
    yReal z;

};

#endif // __QUATERNION_H__


