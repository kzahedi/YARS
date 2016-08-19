#ifndef __QTOGREWINDOW_H__
#define __QTOGREWINDOW_H__

// copied and adapted from
// http://www.ogre3d.org/tikiwiki/tiki-index.php?page=Integrating+Ogre+into+QT5
 
#include <yars/view/gui/CameraHandler.h>
#include <yars/view/gui/WindowConfiguration.h>
#include <yars/view/gui/SceneGraph.h>
#include <yars/view/gui/SdkQtCameraMan.h>
// #include <yars/view/gui/TextOverlay.h>
#include <yars/configuration/data/DataScreen.h>

#ifdef USE_CAPTURE_VIDEO
#  include <lqt/lqt.h>
#  include <lqt/quicktime.h>
#  include <lqt/colormodels.h>
#endif

#include <QtWidgets/QApplication>
#include <QtGui/QKeyEvent>
#include <QWindow>
#include <QGLWidget>
 
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
    void captureImageFrame();
    void quit();
    void setupOSD();

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
    void __initRenderFrame();
    void __catchedLocally(int i);
    void __toggleFollowing();
    void __previousFollowable();
    void __nextFollowable();
    void __previousFollowMode();
    void __nextFollowMode();
    void __updateCamData();

    int          _index;
    int          _metaKey;
    int          _fps;
    int          _imgCaptureFrameIndex;
    unsigned int _lastTime;
    unsigned int _currentTime;
    unsigned int _lastStep;

    QString _elapsedTimeString;
    QString _fpsString1;
    QString _fpsString2;
    float _printTimeFPS;
    float _printRealTimeFactor;
    unsigned long _printTimeLastStep;

    DataObject*          _followableObject;
    CameraHandler*       _cameraHandler;
    WindowConfiguration* _windowConfiguration;
    Ogre::Viewport*      _viewport;
    Ogre::RenderTexture* _pRenderTex;
    Ogre::TexturePtr     _renderTexture;
    int                  _followableIndex;

    // TextOverlay*         _textOverlay;

#ifdef USE_CAPTURE_VIDEO
    void __toggleCaptureMovie();
    void __closeMovie();
    int  __milliSeconds();
    void __captureMovieFrame();
    void __initMovie();

    quicktime_t           *_mov;

    bool                   _captureRunning;
    unsigned long          _captureStep;
    unsigned long          _capturedTenMinutes;
    unsigned long          _frameIndex;
    unsigned long          _capturingOffset;

    char                  *_buffer;
#endif

    DataScreen*   _data;
    Ogre::Vector3 _cpos;
    Ogre::Vector3 _cdir;
    Ogre::Vector3 _clookAt;
    P3D           _ypos;
    P3D           _ylookAt;
    DataCamera*   _camData;

};
 
#endif // QTOGREWINDOW_H
