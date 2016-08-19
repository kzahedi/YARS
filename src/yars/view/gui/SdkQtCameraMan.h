#ifndef __SdkQtCameraMan_H__ 
#define __SdkQtCameraMan_H__

// File modified to change OIS to Qt KeyEvents

#include "Ogre/OgreCamera.h" 
#include "Ogre/OgreSceneNode.h" 
#include "Ogre/OgreFrameListener.h" 
#include <QKeyEvent> 
#include <QMouseEvent>
#include <QWidget>
#include <QtWidgets>

#include <iostream>

using namespace std;

enum CameraStyle 
{
 CS_FREELOOK,
 CS_ORBIT,
 CS_MANUAL
};

class SdkQtCameraMan
{
 public:
   SdkQtCameraMan(Ogre::Camera* cam)
     : mCamera(0)
     , mTarget(0)
     , mOrbiting(false)
     , mZooming(false)
     , mTopSpeed(5)
     , mVelocity(Ogre::Vector3::ZERO)
     , mGoingForward(false)
     , mGoingBack(false)
     , mGoingLeft(false)
     , mGoingRight(false)
     , mGoingUp(false)
     , mGoingDown(false)
     , mFastMove(false)
  {
    setCamera(cam);
    setStyle(CS_FREELOOK);
    // setStyle(CS_ORBIT);
    // setStyle(CS_MANUAL);
  }

   virtual ~SdkQtCameraMan() {}

   virtual void setCamera(Ogre::Camera* cam)
   {
     mCamera = cam;
   }

   virtual Ogre::Camera* getCamera()
   {
     return mCamera;
   }

   virtual void setTarget(Ogre::SceneNode* target) 
   {
     if (target != mTarget) 
     {
       mTarget = target; if(target) 
       {
         setYawPitchDist(Ogre::Degree(0), Ogre::Degree(15), 150);
         mCamera->setAutoTracking(true,mTarget);
       }
       else 
       {
         mCamera->setAutoTracking(false);
       }
     }
   }

   virtual Ogre::SceneNode* getTarget()
   {
     return mTarget; 
   }

   virtual void setYawPitchDist(Ogre::Radian yaw, Ogre::Radian pitch, Ogre::Real dist) 
   {
     mCamera->setPosition(mTarget->_getDerivedPosition());
     mCamera->setOrientation(mTarget->_getDerivedOrientation());
     mCamera->yaw(yaw);
     mCamera->pitch(-pitch);
     mCamera->moveRelative(Ogre::Vector3(0, 0, dist));
   }

   virtual void setTopSpeed(Ogre::Real topSpeed) 
   {
     mTopSpeed = topSpeed;
   }

   virtual Ogre::Real getTopSpeed() 
   {
     return mTopSpeed;
   }

   virtual void setStyle(CameraStyle style) 
   {
     if (mStyle != CS_ORBIT && style == CS_ORBIT) 
     {
       setTarget(mTarget ? mTarget : mCamera->getSceneManager()->getRootSceneNode());
       mCamera->setFixedYawAxis(true);
       manualStop();
       setYawPitchDist(Ogre::Degree(0), Ogre::Degree(15), 150);

     }
     else if (mStyle != CS_FREELOOK && style == CS_FREELOOK) 
     {
       mCamera->setAutoTracking(false);
       mCamera->setFixedYawAxis(true); 
     }
     else if (mStyle != CS_MANUAL && style == CS_MANUAL) 
     {
       mCamera->setAutoTracking(false);
       manualStop(); 
     }
     mStyle = style;
   }

   virtual CameraStyle getStyle()
   {
     return mStyle;
   }

   virtual void manualStop() 
   {
     if (mStyle == CS_FREELOOK) 
     {
       mGoingForward = false;
       mGoingBack    = false;
       mGoingLeft    = false;
       mGoingRight   = false;
       mGoingUp      = false;
       mGoingDown    = false;
       mVelocity = Ogre::Vector3::ZERO;
     }
   }

   virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt)
   {
     if (mStyle == CS_FREELOOK)
     {
       Ogre::Vector3 accel = Ogre::Vector3::ZERO;
       if (mGoingForward) accel += mCamera->getDirection();
       if (mGoingBack)    accel -= mCamera->getDirection();
       if (mGoingRight)   accel += mCamera->getRight();
       if (mGoingLeft)    accel -= mCamera->getRight();
       if (mGoingUp)      accel += mCamera->getUp();
       if (mGoingDown)    accel -= mCamera->getUp();

       if (mGoingForward || mGoingBack) accel[1] = 0.0;
       if (mGoingUp      || mGoingDown) accel[0] = accel[2] = 0.0;

       Ogre::Real topSpeed = mFastMove ? mTopSpeed * 20 : mTopSpeed;
       if (accel.squaredLength() != 0)
       {
         accel.normalise();
         mVelocity += accel * topSpeed * evt.timeSinceLastFrame * 10;
       }
       mVelocity -= mVelocity * evt.timeSinceLastFrame * 10;
       Ogre::Real tooSmall = std::numeric_limits<Ogre::Real>::epsilon();
       if (mVelocity.squaredLength() > topSpeed * topSpeed)
       {
         mVelocity.normalise();
         mVelocity *= topSpeed;
       }
       else if (mVelocity.squaredLength() < tooSmall * tooSmall)
         mVelocity = Ogre::Vector3::ZERO;

       if (mVelocity != Ogre::Vector3::ZERO)
       {
         mCamera->move(mVelocity * evt.timeSinceLastFrame);
       }
     }
     return true;
   }

   virtual void injectKeyDown(const QKeyEvent& evt) 
   {
     if (mStyle == CS_FREELOOK) 
     {
       if      (evt.key() == Qt::Key_W || evt.key() == Qt::Key_Up)       mGoingForward = true;
       else if (evt.key() == Qt::Key_S || evt.key() == Qt::Key_Down)     mGoingBack    = true;
       else if (evt.key() == Qt::Key_A || evt.key() == Qt::Key_Left)     mGoingLeft    = true;
       else if (evt.key() == Qt::Key_D || evt.key() == Qt::Key_Right)    mGoingRight   = true;
       else if (evt.key() == Qt::Key_Q || evt.key() == Qt::Key_PageUp)   mGoingUp      = true;
       else if (evt.key() == Qt::Key_E || evt.key() == Qt::Key_PageDown) mGoingDown    = true;
       else if (evt.key() == Qt::Key_Shift) mFastMove = true;
     }
   }

   virtual void injectKeyUp(const QKeyEvent& evt) 
   {
     if (mStyle == CS_FREELOOK) 
     {
       if      (evt.key() == Qt::Key_W || evt.key() == Qt::Key_Up)       mGoingForward = false;
       else if (evt.key() == Qt::Key_S || evt.key() == Qt::Key_Down)     mGoingBack    = false;
       else if (evt.key() == Qt::Key_A || evt.key() == Qt::Key_Left)     mGoingLeft    = false;
       else if (evt.key() == Qt::Key_D || evt.key() == Qt::Key_Right)    mGoingRight   = false;
       else if (evt.key() == Qt::Key_Q || evt.key() == Qt::Key_PageUp)   mGoingUp      = false;
       else if (evt.key() == Qt::Key_E || evt.key() == Qt::Key_PageDown) mGoingDown    = false;
       else if (evt.key() == Qt::Key_Shift) mFastMove = false;
     }
   }

   virtual void injectMouseMove(int relX, int relY)
   {
     if (mStyle == CS_ORBIT)
     {
       Ogre::Real dist = (mCamera->getPosition() - mTarget->_getDerivedPosition()).length();
       if (mOrbiting)
       {
         mCamera->setPosition(mTarget->_getDerivedPosition());
         mCamera->yaw(Ogre::Degree(-relX * 0.025f));
         mCamera->pitch(Ogre::Degree(-relY * 0.025f));
         mCamera->moveRelative(Ogre::Vector3(0, 0, dist));
       }
       else if (mZooming)
       {
         mCamera->moveRelative(Ogre::Vector3(0, 0, relY * 0.004f * dist));
       }
     }
     else if (mStyle == CS_FREELOOK)
     {
       mCamera->yaw(Ogre::Degree(-relX * 0.15f));
       mCamera->pitch(Ogre::Degree(-relY * 0.15f));
     }
   }

   virtual void injectMouseMoveRightButton(int relX, int relY)
   {
     if (mStyle == CS_FREELOOK)
     {
       Ogre::Vector3 accel(0,1,0);
       if(relY > 0.0) accel[1] = -1.0 - relY;
       else           accel[1] =  1.0 - relY;
       mVelocity += accel;
     }
   }

   virtual void gesture(const QNativeGestureEvent* event) 
   {
     double v = 0.0;
     Ogre::Vector3 accel(0,1,0);
     switch(event->gestureType())
     {
       case Qt::ZoomNativeGesture:
         v = event->value();
         if(v > 0.0) accel[1] = -1.0 - v;
         else        accel[1] =  1.0 - v;
         break;
       default:
         break;
     }
     mVelocity += accel;
   }

   virtual void injectWheelMove(const QWheelEvent& evt) 
   {
     QPoint numPixels = evt.pixelDelta() / 10;
     QPoint numDegrees = evt.angleDelta() / 8;
     Ogre::Vector3 accel = Ogre::Vector3::ZERO;

     float rx = 0.0;
     float ry = 0.0;

     if (!numPixels.isNull())
     {
       rx = numPixels.rx();
       ry = numPixels.ry();
     }
     else if (!numDegrees.isNull())
     {
       QPoint numSteps = numDegrees / 15;
       rx = numSteps.rx();
       ry = numSteps.ry();
     }

     accel += mCamera->getDirection() * ry;
     accel += mCamera->getRight()     * -rx;

     accel[1] = 0.0;

     mVelocity += accel;
     // mCamera->move(mVelocity * evt.timeSinceLastFrame);
   }

   virtual void injectMouseDown(const QMouseEvent& evt) 
   {
     if (mStyle == CS_ORBIT) 
     {
       if (evt.buttons() & Qt::LeftButton)
         mOrbiting = true;
       else if (evt.buttons() & Qt::RightButton)
         mZooming = true;
     }
   }

   virtual void injectMouseUp(const QMouseEvent& evt) 
   {
     if (mStyle == CS_ORBIT) 
     {
       if (evt.buttons() & Qt::LeftButton)
         mOrbiting = false;
       else if (evt.buttons() & Qt::RightButton)
         mZooming = false;
     } 
   }
 protected:

   Ogre::Camera* mCamera;
   CameraStyle mStyle;
   Ogre::SceneNode* mTarget;
   bool mOrbiting;
   bool mZooming;
   Ogre::Real mTopSpeed;
   Ogre::Vector3 mVelocity;
   bool mGoingForward;
   bool mGoingBack;
   bool mGoingLeft;
   bool mGoingRight;
   bool mGoingUp;
   bool mGoingDown;
   bool mFastMove;
};

#endif
