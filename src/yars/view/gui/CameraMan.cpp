#include "CameraMan.h"

#include "view/gui/CameraFactory.h"
#include "configuration/data/Data.h"

#define __CURRENT_CAM  _followCameras[_windowConfiguration->followMode]
#define __CURRENT_DATA Data::instance()->current()->screens()->screen(_windowConfiguration->index)->camera()

CameraMan::CameraMan(WindowConfiguration *windowConfiguration)
{
  _followable = NULL;
  _windowConfiguration = windowConfiguration;
  CameraFactory::create(&_followCameras, _windowConfiguration);
  _windowConfiguration->camNames.clear();

  for(std::vector<FollowCamera*>::iterator i = _followCameras.begin(); i != _followCameras.end(); i++)
  {
    string s;
    (*i)->name(&s);
    _windowConfiguration->camNames.push_back(s);
  }
}

void CameraMan::update(P3D vel)
{
  if(_followable == NULL) return;
  if(_windowConfiguration->followModeChanged)
  {
    _windowConfiguration->followModeChanged = false;
    __CURRENT_CAM->init(_followable, __CURRENT_DATA);
  }
  if(isnan(_followable->pose().position.x) == true) return;

  __CURRENT_CAM->update(vel);

}

void CameraMan::follow(DataObject* followable)
{
  _followable = followable;
  if(_followable == NULL) return;
  __CURRENT_CAM->init(followable, __CURRENT_DATA);
}

void CameraMan::nextFollowMode()
{
  _windowConfiguration->followMode++;
  if(_windowConfiguration->followMode >= (int)_followCameras.size())
  {
    _windowConfiguration->followMode = 0;
  }
  __CURRENT_CAM->init(_followable, __CURRENT_DATA);
}

void CameraMan::previousFollowMode()
{
  _windowConfiguration->followMode--;
  if(_windowConfiguration->followMode < 0)
  {
    _windowConfiguration->followMode = _followCameras.size() - 1;
  }
  __CURRENT_CAM->init(_followable, __CURRENT_DATA);
}

void CameraMan::reset()
{
  _lastGeomPosition                            = 0.0;
  _distanceToFollowedObject                    = 0.0;
  _desiredDynamicTrackingPosition              = 0.0;
  _desiredDynamicTrackingPositionLastOffset    = 0.0;
  _desiredDynamicTrackingPositionCurrentOffset = 0.0;
  _oldAlphaBlendingPosition                    = 0.0;
  _estimatedOrientation                        = 0.0;
  _initialDistance                             = 0.0;
  _orientationEstimationIndex                  = 0;
  _lastCamPosition                             = 0.0;
  _currentCamPosition                          = 0.0;
  _lastAlphaRobotPos                           = 0.0;
  _alphaBlendingInitialDistance                = 0.0;
  _lastDistanceOffset                          = 0.0;
}
