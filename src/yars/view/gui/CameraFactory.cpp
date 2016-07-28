#include "CameraFactory.h"
#include "FollowCamera.h"
#include "OffsetCam.h"
#include "TripodCam.h"
#include "OrbitCam.h"
#include "CenterOrbitCam.h"

void CameraFactory::create(std::vector<FollowCamera*> *list, WindowConfiguration *configuration)
{
  list->clear();
  list->push_back(new OrbitCam(configuration));
  list->push_back(new TripodCam(configuration));
  list->push_back(new OffsetCam(configuration));
  list->push_back(new CenterOrbitCam(configuration));
}
