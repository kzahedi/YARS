#ifndef __SDL_WINDOW_H__
#define __SDL_WINDOW_H__

#include <yars/configuration/data/Data.h>
#include <yars/view/gui/CameraMan.h>
#include <yars/view/gui/WindowConfiguration.h>
#include <yars/view/gui/SceneGraph.h>
#include <yars/view/gui/TextOverlay.h>
#include <yars/view/gui/OgreHandler.h>
#include <yars/view/gui/GuiMutex.h>

#include <OGRE/Ogre.h>
#include <SDL2/SDL.h>

#ifdef USE_CAPTURE_VIDEO
#include <yars/view/gui/VideoCapture.h>
#endif // USE_CAPTURE_VIDEO

#include <pthread.h>
#include <functional>

/** \brief SDL window wrapper for YARS GUI.
 *
 * Manages SDL window, OGRE rendering, and user input.
 */
class SdlWindow
{
public:
  using CloseCallback = std::function<void()>;
  using SyncToggleCallback = std::function<void()>;

  SdlWindow(int index);
  ~SdlWindow();

  void reset();
  void quit();
  void step();
  void swapBuffers();
  void handleEvent(SDL_Event &event);
  bool visible();
  void wait();
  bool added();
  void setAdded();
  bool closed();
  void close();

  void captureVideo();

  /** \brief Set callback for window close. */
  void setCloseCallback(CloseCallback callback) { _closeCallback = std::move(callback); }

  /** \brief Set callback for sync toggle. */
  void setSyncToggleCallback(SyncToggleCallback callback) { _syncToggleCallback = std::move(callback); }
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
    int __milliSeconds();
    void __captureMovieFrame();
    void __initMovie();

    VideoCapture *_videoCapture;

    bool _captureRunning;
    unsigned long _captureStep;
    unsigned long _capturedTenMinutes;
    unsigned long _frameIndex;
    unsigned long _capturingOffset;
#endif

    Ogre::RenderTexture *_pRenderTex;
    Ogre::TexturePtr _renderTexture;

    Ogre::RenderWindow *_window;
    Ogre::Camera *_camera;
    Ogre::SceneNode *_cameraNode;  // OGRE 14: cameras attach to nodes
    Ogre::Viewport *_viewport;
    Ogre::SceneManager *_sceneManager;

    DataObject *_followableObject;
    DataScreen *_data;
    DataCamera *_camData;
    int _followableIndex;
    WindowConfiguration *_windowConfiguration;
    CameraMan *_cameraMan;
    bool _leftMousePressed;
    bool _middleMousePressed;
    bool _rightMousePressed;
    bool _shiftPressed;
    bool _ctrlPressed;
    bool _altPressed;
    bool _metaPressed;
    double _orbitDistance;      // Distance from camera to look-at point
    Ogre::Vector3 _orbitCenter; // The point the camera orbits around
    bool _imgCaptureRunning;
    int _imgCaptureFrameIndex;
    int _fps;
    unsigned int _lastTime;
    unsigned int _currentTime;
    unsigned int _lastStep;
    TextOverlay *_textOverlay;
    stringstream _fpsString;
    stringstream _sst;
    int _index;
    int _shadowMode;
    int _nextShadowMode;
    string _statsString;
    string _timeString;
    string _robotString;
    string _legendString;

    OgreHandler *_ogreHandler;

    Ogre::Vector3 _cpos;
    Ogre::Vector3 _cdir;
    Ogre::Vector3 _clookAt;
    Ogre::Vector3 _cameraVelocity;
    P3D _ypos;
    P3D _ylookAt;
    P3D _camVelocity;
    P3D _camAngularVelocity;
    Uint32 _windowID;
    SDL_Window *_sdlWindow;
    SDL_GLContext _glContext;
    bool _visible;
    bool _added;
    bool _closed;
    CloseCallback _closeCallback;
    SyncToggleCallback _syncToggleCallback;
};

#endif // __SDL_WINDOW_H__
