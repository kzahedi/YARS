#ifndef __FOLLOW_CAMERA_H__
#define __FOLLOW_CAMERA_H__

#include <yars/view/gui/WindowConfiguration.h>
#include <yars/configuration/data/DataObject.h>
#include <yars/configuration/data/DataCamera.h>
#include <yars/defines/types.h>

#include <string>

using namespace std;

/**
 * \brief This class needs to be inherited by following cameras implementations
 */

class FollowCamera
{
  public:
    FollowCamera(WindowConfiguration *windowConfiguraiton, string name);
    virtual ~FollowCamera() { };

    void     setCamera(P3D position, P3D lookAt);
    void     moveCamera(P3D offset);
    void     updateFollowablePosition();
    void     updatePositionOffset();
    void     updateFollowableOrientation();
    void     name(string *name);
    void     updateFollowableName();
    void     setPosition(P3D position);
    yReal distXY(P3D a, P3D b);

    virtual void init(DataObject *f, DataCamera *cam) = 0;
    virtual void update(P3D currentCamVelocity) = 0;

  protected:
    DataObject          *followable;
    WindowConfiguration *configuration;
    P3D                  followablePosition;
    P3D                  followableOrientation;
    P3D                  positionOffset;
    string               followableName;

  private:
    string              _myName;
};

#endif // __FOLLOW_CAMERA_H__


