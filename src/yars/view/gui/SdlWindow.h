#ifndef __SDL_WINDOW_H__
#define __SDL_WINDOW_H__

#include <pthread.h>
#include <OGRE/Ogre.h>

#include "util/Observable.h"
#include "util/ObservableMessage.h"

#include "configuration/data/Data.h"

#include "view/gui/CameraHandler.h"
#include "view/gui/WindowConfiguration.h"
#include "view/gui/SceneGraph.h"
#include "view/gui/TextOverlay.h"
#include "view/gui/OgreHandler.h"

#ifdef USE_CAPTURE_VIDEO
#  include <lqt/lqt.h>
#  include <lqt/quicktime.h>
#  include <lqt/colormodels.h>
#endif // USE_CAPTURE_VIDEO

#ifndef __APPLE__
namespace _SDL_
{
#endif // __APPLE__
# include <SDL2/SDL.h>
# include <SDL2/SDL_opengl.h>
# include <SDL2/SDL_syswm.h>
# include <SDL2/SDL_thread.h>
#ifndef __APPLE__
};

using namespace _SDL_;
#endif // __APPLE__


#include <pthread.h>

class SdlWindow : public Observable
{
  public:
    SdlWindow(int index);
    ~SdlWindow();

    void reset();
    void quit();
    void step();
    void close();

    void captureVideo();
#ifdef USE_CAPTURE_VIDEO
    bool captureRunning();
    void startCaptureVideo();
    void stopCaptureVideo();
#endif // USE_CAPTURE_VIDEO
    void toggleShadows();
    void setupOSD();
    void parseEvent(SDL_Event);

  private:
    
    void __osd();
    void __setupSDL();
    void __processKeyEvent(char, int);
    void __toggleWriteFrames();
    void __openNewWindow();
    void __toggleFollowing();
    void __nextFollowMode();
    void __previousFollowMode();
    void __nextFollowable();
    void __previousFollowable();
    void __captureImageFrame();
    void __initRenderFrame();

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
    CameraHandler       *_cameraHandler;
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
    int                  _index;
    int                  _shadowMode;
    int                  _nextShadowMode;

    OgreHandler         *_ogreHandler;

    Ogre::Vector3        _cpos;
    Ogre::Vector3        _cdir;
    Ogre::Vector3        _clookAt;
    int                  _windowID;
    P3D                  _ypos;
    P3D                  _ylookAt;
};

#endif // __SDL_WINDOW_H__
