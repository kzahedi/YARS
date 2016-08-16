#ifndef __QTOGREWINDOW_H__
#define __QTOGREWINDOW_H__

// copied and adapted from
// http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Integrating+Ogre+into+QT5
 
/*
Qt headers
*/

#include <yars/view/gui/CameraHandler.h>
#include <yars/view/gui/WindowConfiguration.h>
#include <yars/view/gui/SceneGraph.h>

#ifdef USE_CAPTURE_VIDEO
#  include <lqt/lqt.h>
#  include <lqt/quicktime.h>
#  include <lqt/colormodels.h>
#endif // USE_CAPTURE_VIDEO

#include <QtWidgets/QApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QWindow>


 
/*
Ogre3D header
*/
#include <OGRE/Ogre.h>
 
/*
Changed SdkCameraMan implementation to work with QKeyEvent, QMouseEvent, QWheelEvent
*/
// #include "SdkQtCameraMan.h"
 
/*
With the headers included we now need to inherit from QWindow.
*/
class QtOgreWindow : public QWindow, public Ogre::FrameListener
{
  /*
  A QWindow still inherits from QObject and can have signals/slots; we need to add the appropriate
  Q_OBJECT keyword so that Qt's intermediate compiler can do the necessary wireup between our class
  and the rest of Qt.
  */
  Q_OBJECT
 
public:
  explicit QtOgreWindow(int index, QWindow *parent = NULL);
  ~QtOgreWindow();
 
  /*
  We declare these methods virtual to allow for further inheritance.
  */
  virtual void render(QPainter *painter);
  virtual void render();
  virtual void initialize();
  virtual void createScene();
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
  virtual void createCompositor();
#endif
 
  void setAnimating(bool animating);
 
public slots:
 
  virtual void renderLater();
  virtual void renderNow();
 
  /*
  We use an event filter to be able to capture keyboard/mouse events. More on this later.
  */
  virtual bool eventFilter(QObject *target, QEvent *event);
 
signals:
  /*
  Event for clicking on an entity.
  */
  void entitySelected(Ogre::Entity* entity);
 
protected:
  /*
  Ogre3D pointers added here. Useful to have the pointers here for use by the window later.
  */
  Ogre::Root* m_ogreRoot;
  Ogre::RenderWindow* m_ogreWindow;
  Ogre::SceneManager* m_ogreSceneMgr;
  Ogre::Camera* m_ogreCamera;
  Ogre::ColourValue m_ogreBackground;
  // OgreQtBites::SdkQtCameraMan* m_cameraMan;
 
  bool m_update_pending;
  bool m_animating;
 
  /*
  The below methods are what is actually fired when they keys on the keyboard are hit.
  Similar events are fired when the mouse is pressed or other events occur.
  */
  virtual void keyPressEvent(QKeyEvent * ev);
  virtual void keyReleaseEvent(QKeyEvent * ev);
  virtual void mouseMoveEvent(QMouseEvent* e);
  virtual void wheelEvent(QWheelEvent* e);
  virtual void mousePressEvent(QMouseEvent* e);
  virtual void mouseReleaseEvent(QMouseEvent* e);
  virtual void exposeEvent(QExposeEvent *event);
  virtual bool event(QEvent *event);
 
  /*
  FrameListener method
  */
  virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
 
  /*
  Write log messages to Ogre log
  */
  void log(Ogre::String msg);
  void log(QString msg);

  private:
    int                  _index;
    WindowConfiguration* _windowConfiguration;

};
 
#endif // QTOGREWINDOW_H
