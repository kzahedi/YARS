#ifndef __POSE_H__
#define __POSE_H__

#include <yars/defines/types.h>
#include <yars/types/P3D.h>
#include <yars/util/macros.h>
#include <yars/types/Quaternion.h>

#include <ostream>
#include <iostream>
#include <math.h>

# define POSE_ANGLE_TYPE_RAD 0
# define POSE_ANGLE_TYPE_DEG 1

namespace yars {

class Pose
{
  public:
    P3D          position;
    P3D          orientation;
    Quaternion q;

    Pose();
    Pose(double x, double y, double z, double qx, double qy, double qz, double qw);
    Pose(double x, double y, double z, double rx, double ry, double rz);
    Pose(const Pose &p);

    friend std::ostream& operator<<(std::ostream& str, const Pose& p)
    {
      str << p.position.x << ", " << p.position.y << ", " << p.position.z << ", " <<
             RAD_TO_DEG(p.orientation.x) << ", " << RAD_TO_DEG(p.orientation.y) << ", " << RAD_TO_DEG(p.orientation.z);
      return str;
    };

    P3D deg();

    void toRad();
    void toDeg();

    Pose & operator+=(const Pose &p);
    Pose & operator=(const Pose &b);
    void operator<<(const Pose &p);

};

} // namespace yars

// Temporary global using directive for backward compatibility during namespace transition
using namespace yars;

#endif // __POSE_H__
