#ifndef __DATA_POSE_FACTORY_H__
#define __DATA_POSE_FACTORY_H__

#include <yars/types/Pose.h>
#include <yars/configuration/data/DataParseElement.h>

#include <yars/defines/ymath.h>

# define YARS_STRING_X      (char*)"x"
# define YARS_STRING_Y      (char*)"y"
# define YARS_STRING_Z      (char*)"z"
# define YARS_STRING_ALPHA  (char*)"alpha"
# define YARS_STRING_BETA   (char*)"beta"
# define YARS_STRING_GAMMA  (char*)"gamma"
# define YARS_STRING_TYPE   (char*)"type"
# define YARS_STRING_DEGREE (char*)"deg"

class DataPoseFactory
{
  public:
    static void set(Pose &pose, DataParseElement *element)
    {
      element->set(YARS_STRING_X,     pose.position.x);
      element->set(YARS_STRING_Y,     pose.position.y);
      element->set(YARS_STRING_Z,     pose.position.z);
      element->set(YARS_STRING_ALPHA, pose.orientation.x);
      element->set(YARS_STRING_BETA,  pose.orientation.y);
      element->set(YARS_STRING_GAMMA, pose.orientation.z);

      if(element->attribute("type"))
      {
        if(element->attribute("type")->value() == "deg")
        {
          pose.toRad(); // because internally we operate in rad, and data was given in deg
        }
        else
        {
          // already given in rad. no conversion necessary
        }
      }
      else
      {
        pose.toRad(); // default input is degree
      }
      pose.q << pose.orientation;
    }
};

#endif // __DATA_POSE_FACTORY_H__


