#ifndef __SDL_WINDOW_H__
#define __SDL_WINDOW_H__

#include <OGRE/Ogre.h>
#include <yars/util/Observable.h>
#include <yars/util/ObservableMessage.h>
#include "GuiMutex.h"

#include <yars/configuration/data/Data.h>

#include <yars/view/gui/CameraMan.h>
#include <yars/view/gui/WindowConfiguration.h>
#include <yars/view/gui/SceneGraph.h>
#include <yars/view/gui/TextOverlay.h>
#include <yars/view/gui/OgreHandler.h>

#include "SDL2/SDL.h"

#ifdef USE_CAPTURE_VIDEO
#  include <lqt/lqt.h>
#  include <lqt/quicktime.h>
#  include <lqt/colormodels.h>
#endif // USE_CAPTURE_VIDEO

#include <pthread.h>

class SdlWindow : public Observable
{
  public:
    SdlWindow(int index);
    ~SdlWindow();

    void reset();
    void quit();
    void step();
    void handleEvent(SDL_Event &event);
    bool visible();
    void wait();
    bool added();
    void setAdded();
    bool closed();

    void captureVideo();
#ifdef USE_CAPTURE_VIDEO
    bool captureRunning();
    void startCaptureVideo();
    void stopCaptureVideo();
#endif // USE_CAPTURE_VIDEO
    void toggleShadows();
    void setupOSD();

  private:
    
    void __osd();
    void __setupSDL();
    void __processKeyEvent(char, int);
    void __toggleWriteFrames();
    // void __openNewWindow();
    void __toggleFollowing();
    void __nextFollowMode();
    void __previousFollowMode();
    void __nextFollowable();
    void __previousFollowable();
    void __captureImageFrame();
    void __initRenderFrame();
    void __handleFingerUp(SDL_Event &event);
    void __handleFingerDown(SDL_Event &event);

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

    Ogre::RenderTexture *_pRenderTex;
    Ogre::TexturePtr     _renderTexture;

    Ogre::RenderWindow  *_window;
    Ogre::Camera        *_camera;
    Ogre::Viewport      *_viewport;
    Ogre::SceneManager  *_sceneManager;

    DataObject          *_followableObject;
    DataScreen          *_data;
    DataCamera          *_camData;
    int                  _followableIndex;
    WindowConfiguration *_windowConfiguration;
    CameraMan           *_cameraMan;
    bool                 _mousePressed;
    bool                 _shiftPressed;
    bool                 _ctrlPressed;
    bool                 _altPressed;
    bool                 _metaPressed;
    bool                 _imgCaptureRunning;
    int                  _imgCaptureFrameIndex;
    int                  _fps;
    unsigned int         _lastTime;
    unsigned int         _currentTime;
    unsigned int         _lastStep;
    TextOverlay         *_textOverlay;
    stringstream         _fpsString;
    stringstream         _sst;
    int                  _index;
    int                  _shadowMode;
    int                  _nextShadowMode;
    string               _statsString;
    string               _timeString;
    string               _robotString;
    string               _legendString;

    OgreHandler         *_ogreHandler;

    Ogre::Vector3        _cpos;
    Ogre::Vector3        _cdir;
    Ogre::Vector3        _clookAt;
    Ogre::Vector3        _cameraVelocity;
    P3D                  _ypos;
    P3D                  _ylookAt;
    P3D                  _camVelocity;
    Uint32               _windowID;
    SDL_Window*          _sdlWindow;
    bool                 _visible;
    bool                 _added;
    bool                 _closed;
};

#endif // __SDL_WINDOW_H__
