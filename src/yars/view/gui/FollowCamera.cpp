#include "FollowCamera.h"
#include <yars/util/macros.h>

FollowCamera::FollowCamera(WindowConfiguration *c, string name)
{
  this->configuration = c;
  followable          = NULL;
  _myName             = name;
}

void FollowCamera::moveCamera(P3D offset)
{
  configuration->cameraPosition += offset;
}

void FollowCamera::setCamera(P3D position, P3D angles)
{
  configuration->cameraPosition = position;
}

void FollowCamera::updateFollowablePosition()
{
  followablePosition = followable->pose().position;
}

void FollowCamera::updateFollowableOrientation()
{
  followableOrientation = followable->pose().orientation;
}

void FollowCamera::updatePositionOffset()
{
  P3D x;
  x = followable->pose().position;
  positionOffset = configuration->cameraPosition - x;
}

void FollowCamera::setPosition(P3D position)
{
  configuration->cameraPosition = position;
}

void FollowCamera::name(string *name)
{
  *name = _myName;
}

void FollowCamera::updateFollowableName()
{
  followableName = followable->name();
}

yReal FollowCamera::distXY(P3D a, P3D b)
{
  yReal x = a.x - b.x;
  yReal y = a.y - b.y;
  return sqrt( x*x + y*y );
}

// yReal FollowCamera::__wrapAngle(yReal a, yReal b)
// {
//   if(fabs(a - b) < 180) return a;
//   if(a < b)
//   {
//     while(fabs(a - b) > 180) a += 360;
//   }
//   else
//   {
//     while(fabs(a - b) > 180) a -= 360;
//   }
//   return a;
// }
