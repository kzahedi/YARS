#include <yars/view/gui/CameraFactory.h>
#include <yars/view/gui/FollowCamera.h>
#include <yars/view/gui/OffsetCam.h>
#include <yars/view/gui/TripodCam.h>
#include <yars/view/gui/OrbitCam.h>
#include <yars/view/gui/CenterOrbitCam.h>

void CameraFactory::create(std::vector<FollowCamera*> *list, WindowConfiguration *configuration)
{
  list->clear();
  list->push_back(new OrbitCam(configuration));
  list->push_back(new TripodCam(configuration));
  list->push_back(new OffsetCam(configuration));
  list->push_back(new CenterOrbitCam(configuration));
}
