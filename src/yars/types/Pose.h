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

class Pose
{
  public:
    P3D          position;
    P3D          orientation;
    ::Quaternion q;

    Pose();
    Pose(yReal x, yReal y, yReal z, yReal qx, yReal qy, yReal qz, yReal qw);
    Pose(yReal x, yReal y, yReal z, yReal rx, yReal ry, yReal rz);

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

#endif // __POSE_H__
