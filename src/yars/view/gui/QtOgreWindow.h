#ifndef __QTOGREWINDOW_H__
#define __QTOGREWINDOW_H__

// copied and adapted from
// http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Integrating+Ogre+into+QT5
 
#include <yars/view/gui/CameraHandler.h>
#include <yars/view/gui/WindowConfiguration.h>
#include <yars/view/gui/SceneGraph.h>
#include <yars/view/gui/SdkQtCameraMan.h>

#ifdef USE_CAPTURE_VIDEO
#  include <lqt/lqt.h>
#  include <lqt/quicktime.h>
#  include <lqt/colormodels.h>
#endif

#include <QtWidgets/QApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QWindow>
 
#include <OGRE/Ogre.h>
 
class QtOgreWindow : public QWindow, public Ogre::FrameListener
{
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
#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
    virtual void createCompositor();
#endif
   
    void setAnimating(bool animating);

#ifdef USE_CAPTURE_VIDEO
    bool captureRunning();
    void startCaptureVideo();
    void stopCaptureVideo();
    void captureVideo();
#endif // USE_CAPTURE_VIDEO
   
  public slots:
   
    virtual void renderLater();
    virtual void renderNow();
   
    virtual bool eventFilter(QObject *target, QEvent *event);
   
  signals:
    // void signalWindowConfigurationChanged();

    // void toggleFollowMode();

    // void previousFollowable();
    // void nextFollowable();

    // void previousFollowMode();
    // void nextFollowMode();

    // void captureVideo();
    // void writeFrames();

    // void openNewWindow();

    // void setWindowTitle();
    // void setWindowSize();
 
  protected:
    /*
    Ogre3D pointers added here. Useful to have the pointers here for use by the window later.
    */
    Ogre::Root* _ogreRoot;
    Ogre::RenderWindow* _ogreWindow;
    Ogre::SceneManager* _ogreSceneMgr;
    Ogre::Camera* _ogreCamera;
    Ogre::ColourValue _ogreBackground;
    SdkQtCameraMan* _cameraMan;
   
    bool _update_pending;
    bool _animating;
   
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
    void __catchedLocally(int key);

    int          _index;
    int          _metaKey;
    int          _fps;
    unsigned int _lastTime;
    unsigned int _currentTime;
    unsigned int _lastStep;

    WindowConfiguration* _windowConfiguration;
    Ogre::Viewport*      _viewport;
    Ogre::RenderTexture* _pRenderTex;
    Ogre::TexturePtr     _renderTexture;

#ifdef USE_CAPTURE_VIDEO
    void __toggleCaptureMovie();
    void __closeMovie();
    int  __milliSeconds();
    void __captureMovieFrame();
    void __initMovie();
    void __initRenderFrame();

    quicktime_t           *_mov;

    bool                   _captureRunning;
    unsigned long          _captureStep;
    unsigned long          _capturedTenMinutes;
    unsigned long          _frameIndex;
    unsigned long          _capturingOffset;

    char                  *_buffer;
#endif

};
 
#endif // QTOGREWINDOW_H
