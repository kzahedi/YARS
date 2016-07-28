#ifndef __CAMERA_FACTORY_H__
#define __CAMERA_FACTORY_H__

#include <yars/view/gui/FollowCamera.h>
#include <yars/view/gui/WindowConfiguration.h>

class CameraFactory
{
  public:
    static void create(std::vector<FollowCamera*> *list, WindowConfiguration *configuration);

};

#endif // __CAMERA_FACTORY_H__


