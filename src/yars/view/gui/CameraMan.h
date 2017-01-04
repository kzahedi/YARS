#ifndef __CAMERA_HANDLER_H__
#define __CAMERA_HANDLER_H__

#include <yars/view/gui/WindowConfiguration.h>
#include <yars/view/gui/FollowCamera.h>
#include <yars/types/P3D.h>


class CameraMan
{
  public:
    CameraMan(WindowConfiguration *windowConfiguration);
    void nextFollowMode();
    void previousFollowMode();
    void follow(DataObject *followable);
    void update(P3D velo);
    void reset();

  private:

    void __moveCamera(P3D offset);

    double __wrapAngle(double a, double b);

    DataObject        *_followable;
    P3D                _lastGeomPosition;
    P3D                _distanceToFollowedObject;
    P3D                _desiredDynamicTrackingPosition;
    P3D                _desiredDynamicTrackingPositionLastOffset;
    P3D                _desiredDynamicTrackingPositionCurrentOffset;
    P3D                _oldAlphaBlendingPosition;

    // dynamic tracking variables
    P3D                _estimatedOrientation;
    P3D                _initialDistance;
    int                _orientationEstimationIndex;
    P3D                _lastCamPosition;
    P3D                _currentCamPosition;

    // alpha
    P3D                _lastAlphaRobotPos;
    P3D                _alphaBlendingInitialDistance;
    P3D                _lastDistanceOffset;

    WindowConfiguration *_windowConfiguration;

    std::vector<FollowCamera*> _followCameras;

};

#endif // __CAMERA_H__
