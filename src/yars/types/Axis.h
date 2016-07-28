#ifndef __AXIS_H__
#define __AXIS_H__

#include "P3D.h"
#include "Pose.h"

#include <vector>

using namespace std;

class Axis : public ::Pose
{
  public:
    P3D  scale;

    Axis & operator=(const Axis &b)
    {
      scale = b.scale;
      position = b.position;
      orientation = b.orientation;
      return *this;
    }

};

typedef std::vector<Axis> Axes;

#endif // __AXIS_H__

