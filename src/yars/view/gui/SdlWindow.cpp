#include "SdlWindow.h"

#include <yars/configuration/data/Data.h>
#include <yars/configuration/YarsConfiguration.h>
#include <yars/view/gui/KeyHandler.h>
#include <yars/view/console/ConsoleView.h>
#include <yars/util/macros.h>
#include <yars/util/OSD.h>

#include <OGRE/Ogre.h>

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

#include <math.h>


#define FACTOR 0.01

#define __NO_KEY  0
#define __CONTROL 1
#define __ALT     2
#define __SHIFT   4

#define __SHADOWTYPE_STENCIL_ADDITIVE              0
#define __SHADOWTYPE_STENCIL_MODULATIVE            1
#define __SHADOWTYPE_TEXTURE_ADDITIVE              2
#define __SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED   3
#define __SHADOWTYPE_TEXTURE_MODULATIVE            4
#define __SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED 5
#define __SHADOWTYPE_NONE                          6

# define CHECK_IF_THERE_ARE_FOLLOWABLES \
  if(Data::instance()->current()->screens()->followables()           == NULL) return; \
if(Data::instance()->current()->screens()->followables()->o_size() == 0) return;

# define GET_FOLLOWABLE(a) \
  Data::instance()->current()->screens()->followables()->followable(a)

// bad SDL, bad.. seems to be Windows only at least
#ifdef main
#undef main
#endif

//#include <boost/thread.hpp>

#include <string>

#ifdef __APPLE__
#include "OSX_wrap.h"
#endif

#ifdef USE_CAPTURE_VIDEO
extern "C"
{
#  include <libavutil/opt.h>
#  include <libavutil/imgutils.h>
#  include <libswscale/swscale.h>
}
#endif // USE_CAPTURE_VIDEO


using namespace std;

SdlWindow::SdlWindow(int index)
  : _visible(false),
  _added(false)
{
  _index                = index;
  _windowConfiguration  = new WindowConfiguration(index);
  _cameraMan            = new CameraMan(_windowConfiguration);
  _data                 = Data::instance()->current()->screens()->screen(index);
  _camData              = Data::instance()->current()->screens()->screen(index)->camera();
  _mousePressed         = false;
  _shiftPressed         = false;
  _ctrlPressed          = false;
  _altPressed           = false;
  _metaPressed          = false;
#ifdef USE_CAPTURE_VIDEO
  _captureRunning       = false;
#endif // USE_CAPTURE_VIDEO
  _imgCaptureRunning    = false;
  _imgCaptureFrameIndex = 0;
  _followableIndex      = 0;
  _closed               = false;
  _fps                  = 0;
  _lastTime             = 0;
  _currentTime          = 0;
  _lastStep             = 0;
  _shadowMode           = __SHADOWTYPE_TEXTURE_ADDITIVE;
  _nextShadowMode       = __SHADOWTYPE_TEXTURE_ADDITIVE+1;
  _cameraVelocity       = Ogre::Vector3::ZERO;
  _camAngularVelocity   = 0.0;
  __setupSDL();
  // __setScene();

  if(_windowConfiguration->useFollow) // start with following
  {
    _windowConfiguration->useFollow = false; // is set to true in __toggleFollowing
    __toggleFollowing();
  }

#ifdef USE_CAPTURE_VIDEO
  // if(__YARS_GET_USE_CAPTURE(index))
  // {
  // _captureRunning = true;
  // __initMovie();
  // }
  _avContext            = NULL;
  _avCodec              = NULL;
  _avFrame              = NULL;
  _avPkt                = NULL;
  _swsContext           = NULL;
  _ycbcr                = new uint8_t[3];

  avcodec_register_all();

#endif // USE_CAPTURE_VIDEO
}

void SdlWindow::wait()
{
  SDL_Event event;
  while(SDL_PollEvent(&event) && _visible == false)
  {
    if(event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SHOWN)
    {
      _visible = true;
    }
    usleep(100);
    if(_visible == true) return;
  }
}

void SdlWindow::step()
{

#ifdef USE_CAPTURE_VIDEO
  if(_captureRunning || _imgCaptureRunning)
#else // USE_CAPTURE_VIDEO
    if(_imgCaptureRunning)
#endif // USE_CAPTURE_VIDEO
    {
      if(_index == 0) _textOverlay->setText(_statsString,"");
    }
    else
    {
      _fps++;
      _currentTime = SDL_GetTicks();
      if(_currentTime > _lastTime + 500)
      {
        int step = __YARS_GET_CONTINOUS_STEP;

        double rt   = (double)(step - _lastStep) / ((double)__YARS_GET_SIMULATOR_FREQUENCY);
        // double f    = (double)_fps;
        double diff = 1000.0/((double)(_currentTime - _lastTime));

        // f  =  f * diff;
        rt = rt * diff;
        _fpsString.str("");

        _fpsString << std::fixed << std::setprecision(2) << rt << " RT\n";
        // _fpsString << std::fixed << std::setprecision(2) << f << " FPS";
        if(_index == 0) _textOverlay->setText(_statsString, _fpsString.str());
        _lastTime = _currentTime;
        _lastStep = step;
        _fps = 0;
      }
    }

  if(_windowConfiguration->useFollow)
  {
    _cpos    = _camera->getPosition();
    _cdir    = _camera->getDirection();
    _clookAt = _cpos;
    for(int i = 0; i < 3; i++) _clookAt[i] += _cdir[i];
    OGRE_TO_YARS(_cpos, _ypos);
    OGRE_TO_YARS(_clookAt, _ylookAt);
    _camData->setPosition(_ypos);
    _camData->setLookAt(_ylookAt);
    OGRE_TO_YARS(_cameraVelocity, _camVelocity);

    _cameraMan->update(_camVelocity);

    YARS_TO_OGRE(_camData->position(), _cpos);
    YARS_TO_OGRE(_camData->lookAt(),   _clookAt);
    _camera->setPosition(_cpos[0], _cpos[1],    _cpos[2]);
    _camera->lookAt(_clookAt[0],   _clookAt[1], _clookAt[2]);
  }
  else if(_cameraVelocity.length() > 0.01 ||
          _camAngularVelocity.length() > 0.0001)
  {
    // cout << _cameraVelocity[0] << " "
    // << _cameraVelocity[1] << " "
    // << _cameraVelocity[2] << endl;

    _camera->yaw(Ogre::Radian(_camAngularVelocity.x   * FACTOR));
    _camera->pitch(Ogre::Radian(_camAngularVelocity.y * FACTOR));

    _camera->moveRelative(_cameraVelocity);

    _cpos    = _camera->getPosition();
    _cdir    = _camera->getDirection();
    _clookAt = _cpos;
    for(int i = 0; i < 3; i++) _clookAt[i] += _cdir[i];
    OGRE_TO_YARS(_cpos, _ypos);
    OGRE_TO_YARS(_clookAt, _ylookAt);
    _camData->setPosition(_ypos);
    _camData->setLookAt(_ylookAt);
  }

  __osd();

  _cameraVelocity *= 0.9;
  _camAngularVelocity *= 0.9;
}

void SdlWindow::handleEvent(SDL_Event &event)
{
  if(event.window.windowID != _windowID) return;

  switch(event.type)
  {
    case SDL_MULTIGESTURE:
      cout << "multi gesture" << endl;
      break;
    case SDL_QUIT:
      break;
    case SDL_KEYDOWN:
      switch (event.key.keysym.sym)
      {
        case SDLK_ESCAPE:
          break;
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
          _shiftPressed = true;
          break;
        case SDLK_RCTRL:
        case SDLK_LCTRL:
          _ctrlPressed = true;
          break;
        case SDLK_RALT:
        case SDLK_LALT:
          _altPressed = true;
          break;
        case SDLK_LGUI:
          _metaPressed = true;
          break;
        default:
          break;
      }
      __processKeyEvent(event.key.keysym.sym, event.key.keysym.mod);
      break;
    case SDL_KEYUP:
      _shiftPressed = false;
      _ctrlPressed  = false;
      _altPressed   = false;
      _metaPressed  = false;
      break;

    case SDL_MOUSEMOTION:
      if(_mousePressed)
      {
        if(_metaPressed && !_altPressed)
        {
          _cameraVelocity[0] += -event.motion.xrel * FACTOR;
          _cameraVelocity[2] += -event.motion.yrel * FACTOR;
        }
        if(!_metaPressed && _altPressed)
        {
          _cameraVelocity[1] += event.motion.yrel * FACTOR;
        }
        if(!_metaPressed && !_altPressed)
        {
          _camAngularVelocity.x += event.motion.xrel * 10.0 * FACTOR;
          _camAngularVelocity.y += event.motion.yrel * 10.0 * FACTOR;
        }
      }
      break;
    case SDL_MOUSEBUTTONUP:
      _mousePressed = false;
      break;
    case SDL_MOUSEBUTTONDOWN:
      _mousePressed = true;
      break;
    case SDL_WINDOWEVENT:
      switch (event.window.event)
      {
        case SDL_WINDOWEVENT_SHOWN:
          _visible = true;
          break;
        case SDL_WINDOWEVENT_CLOSE:
          _closed = true;
          notifyObservers(_m_closeWindow);
          break;
        case SDL_WINDOWEVENT_RESIZED:
          _window->resize(event.window.data1, event.window.data2);
          _window->windowMovedOrResized();
          const Ogre::Real aspectRatio = Ogre::Real(_viewport->getActualWidth())
            / Ogre::Real(_viewport->getActualHeight());
          _camera->setAspectRatio(aspectRatio);

          Ogre::Real x = _viewport->getActualWidth() - 140;
          Ogre::Real y = 10;
          if(_index == 0) _textOverlay->setPosition(_legendString, x, y);
          break;
      }
      // case SDL_MULTIGESTURE:
    case SDL_FINGERUP:
      __handleFingerUp(event);
      // case SDL_FINGERMOTION:
    case SDL_FINGERDOWN:
      __handleFingerDown(event);
      break;
  }

  step();
}

SdlWindow::~SdlWindow()
{
  // cout << "destructor" << endl;
  // SDL_DestroyWindow(_sdlWindow);
  // SDL_Quit();
}


void SdlWindow::__setupSDL()
{
  // SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Init(SDL_INIT_VIDEO);

  if(__YARS_GET_USE_WINDOW_GEOMETRY)
  {
    _sdlWindow = SDL_CreateWindow(_windowConfiguration->name.c_str(),
                                  _windowConfiguration->geometry.x(),
                                  _windowConfiguration->geometry.y(),
                                  _windowConfiguration->geometry.width(),
                                  _windowConfiguration->geometry.height(),
                                  SDL_WINDOW_RESIZABLE); // | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  }
  else
  {
    _sdlWindow = SDL_CreateWindow(_windowConfiguration->name.c_str(),
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  _windowConfiguration->geometry.width(),
                                  _windowConfiguration->geometry.height(),
                                  SDL_WINDOW_RESIZABLE); // | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  }

  if ( _sdlWindow == NULL ) {
    printf( "SDL_CreateWindow failed: %s\n", SDL_GetError() );
    return;
  }

  // SDL_WarpMouse(800/2, 600/2);
  // SDL_WM_GrabInput(SDL_GRAB_OFF);
  // SDL_ShowCursor(SDL_ENABLE);


#ifdef __WINDOWS__
  SDL_GLContext glcontext = NULL;
  glcontext = SDL_GL_CreateContext( _sdlWindow );
  if ( glcontext == NULL ) {
    printf( "SDL_GL_CreateContext failed: %s\n", SDL_GetError() );
    return;
  }
#endif
  SDL_SysWMinfo syswm_info;
  SDL_VERSION( &syswm_info.version );
  if ( !SDL_GetWindowWMInfo(_sdlWindow, &syswm_info ) ) {
    printf( "SDL_GetWindowWMInfo failed.\n" );
    return;
  }

#ifdef __APPLE__
  // I suspect triple buffering is on by default, which makes vsync pointless?
  // except maybe for poorly implemented render loops which will then be forced to wait
  //    ogre_render_window->setVSyncEnabled( false );
#else
  // NOTE: SDL_GL_SWAP_CONTROL was SDL 1.2 and has been retired
  SDL_GL_SetSwapInterval( 1 );
#endif

  Ogre::NameValuePairList params;
#ifdef __WINDOWS__
  params["externalGLControl"] = "1";
  // only supported for Win32 on Ogre 1.8 not on other platforms (documentation needs fixing to accurately reflect this)
  params["externalGLContext"] = Ogre::StringConverter::toString( (unsigned long)glcontext );
  params["externalWindowHandle"] = Ogre::StringConverter::toString( (unsigned long)syswm_info.info.win.window );
#endif
#ifdef __linux__
  // params["externalGLControl"] = "1";
  // params["currentGLContext"] = "1";
  params["parentWindowHandle"] = Ogre::StringConverter::toString( (unsigned long)syswm_info.info.x11.window );
#endif
#ifdef __APPLE__
  params["externalGLControl"] = "1";
  // only supported for Win32 on Ogre 1.8 not on other platforms (documentation needs fixing to accurately reflect this)
  //    params["externalGLContext"] = Ogre::StringConverter::toString( glcontext );
  params["externalWindowHandle"] = OSX_cocoa_view( syswm_info );
  params["macAPI"] = "cocoa";
  params["macAPICocoaUseNSView"] = "true";
#endif

  //params["displayFrequency"] = 100;

  _ogreHandler = OgreHandler::instance();
  stringstream oss;
  oss << "YARS Render window " << _index;
  _window = _ogreHandler->root()->createRenderWindow(oss.str(),
                                                     _windowConfiguration->geometry.width(),
                                                     _windowConfiguration->geometry.height(),
                                                     false,
                                                     &params);

  _window->setActive(true);

  // NOTE: since we are driving with SDL, we need to keep the Ogre side updated for window visibility
  _window->setVisible( true );

  Ogre::Vector3 pos;
  Ogre::Vector3 lookAt;

  YARS_TO_OGRE(_windowConfiguration->cameraPosition, pos);
  YARS_TO_OGRE(_windowConfiguration->cameraLookAt,   lookAt);

  _sceneManager = _ogreHandler->getSceneManager();
  oss.str("");
  oss << "YARS Camera" << _index;
  _camera = _sceneManager->createCamera(oss.str());
  _camera->setNearClipDistance(0.01f);
  _camera->setFarClipDistance(1000000.0f);
  _camera->setPosition(pos[0], pos[1], pos[2]);
  _camera->lookAt(lookAt[0], lookAt[1], lookAt[2]);

  const Ogre::Real aspectRatio = Ogre::Real(_windowConfiguration->geometry.width()) / Ogre::Real(_windowConfiguration->geometry.height());
  _camera->setAspectRatio(aspectRatio);

  _viewport = _window->addViewport(_camera);
  Ogre::ColourValue fadeColour(0.9, 0.9, 0.9);
  // _sceneManager->setFog(Ogre::FOG_EXP2, fadeColour, 0.001, 500.0, 1000.0);
  _viewport->setBackgroundColour(fadeColour);

  _windowID = SDL_GetWindowID(_sdlWindow);

  SDL_RecordGesture(-1);
}


void SdlWindow::setupOSD()
{
  if(_index != 0) return;

  Colour osdColour = _data->osdTimeFontColour();
  string osdFont = _data->osdTimeFontName();
  string fontsize;

  stringstream oss;
  oss << "text overlay " << _index;
  if(_index == 0) _textOverlay  = new TextOverlay(oss.str());

  oss.str("");
  oss << _data->osdTimeFontSize();
  fontsize = oss.str();
  oss.str("");
  oss << "time " << _index;
  _timeString = oss.str();
  if(_index == 0) _textOverlay->addTextBox(_timeString, "00d:00h:00m:00s", 10, 10,  100, 20,
                                           Ogre::ColourValue(osdColour.red(), osdColour.green(), osdColour.blue(), osdColour.alpha()),
                                           osdFont, fontsize);

  oss.str("");
  oss << "stats " << _index;
  _statsString = oss.str();
  if(_index == 0) _textOverlay->addTextBox(_statsString, "", 10, 40,  100, 20,
                                           Ogre::ColourValue(osdColour.red(), osdColour.green(), osdColour.blue(), osdColour.alpha()),
                                           osdFont, "16");

  osdColour = _data->osdRobotFontColour();
  osdFont = _data->osdRobotFontName();
  oss.str("");
  oss << _data->osdRobotFontSize();
  fontsize = oss.str();
  oss.str("");
  oss << "robot " << _index;
  _robotString = oss.str();
  if(_index == 0) _textOverlay->addTextBox(_robotString, "", 10, _viewport->getActualHeight() - _data->osdRobotFontHeight() - 10,
                                           _data->osdRobotFontWidth(), _data->osdRobotFontHeight(),
                                           Ogre::ColourValue(osdColour.red(), osdColour.green(), osdColour.blue(), osdColour.alpha()),
                                           osdFont, fontsize);

  Ogre::Real x = _viewport->getActualWidth() - 140;
  Ogre::Real y = 10;
  oss.str("");
  oss << "legend " << _index;
  _legendString = oss.str();
  if(_index == 0) _textOverlay->addTextBox(_legendString,
                                           "^0YARS, Zahedi", x, y, 15, 10,
                                           Ogre::ColourValue(75.0/255.0, 117.0/255.0, 148.0/255.0,1.0f),
                                           "Legend", "24");
}

void SdlWindow::reset()
{
}

void SdlWindow::quit()
{
#ifdef USE_CAPTURE_VIDEO
  if(_captureRunning) __closeMovie();
#endif // USE_CAPTURE_VIDEO
  // SDL_DestroyWindow(_sdlWindow);
  SDL_Quit();
}

void SdlWindow::__processKeyEvent(char chr, int mod)
{
  bool shift = (mod & KMOD_LSHIFT) || (mod & KMOD_RSHIFT);
#ifdef __APPLE__
  bool cmd   = (mod & KMOD_LGUI)   || (mod & KMOD_RGUI);
#else // __APPLE__
  bool cmd   = (mod & KMOD_LCTRL)  || (mod & KMOD_RCTRL);
#endif // __APPLE__
  bool alt   = (mod & KMOD_LALT)   || (mod & KMOD_RALT);

  switch(KeyHandler::instance()->handleKeyEvent(alt, cmd, shift, chr))
  {
    // case YarsKeyFunction::Quit:
    // close();
    // notifyObservers(_m_quit_called);
    // break;
    case YarsKeyFunction::PrintViewPoint:
      ConsoleView::printViewpoint(_windowConfiguration->cameraPosition, _windowConfiguration->cameraLookAt);
      break;
      // #ifdef USE_CAPTURE_VIDEO
      // case YarsKeyFunction::CaptureVideo:
      // __toggleCaptureMovie();
      // break;
      // #endif // USE_CAPTURE_VIDEO
      // case YarsKeyFunction::WriteFrames:
      // __toggleWriteFrames();
      // break;
      // case YarsKeyFunction::VisualiseAxes:
      // _windowConfiguration->visualiseAxes = !_windowConfiguration->visualiseAxes;
      // break;
      // case YarsKeyFunction::OpenNewWindow:
      // __openNewWindow();
      // break;
      // case YarsKeyFunction::SetWindowTitle:
      // __openWindowTitleDialog();
      // break;
      // case YarsKeyFunction::ShowWindowConfigurationDialog:
      // __showDialog();
      // break;
      // case YarsKeyFunction::SetWindowSize:
      // __setWindowSize();
      // break;
      // case YarsKeyFunction::OnScreenDisplay_FramesPerSecond:
      // _windowConfiguration->osdFramePerSecond = !_windowConfiguration->osdFramePerSecond;
      // break;
      // case YarsKeyFunction::OnScreenDisplay_ElapsedTime:
      // _windowConfiguration->osdElapsedTime = !_windowConfiguration->osdElapsedTime;
      // break;
      // case YarsKeyFunction::OnScreenDisplay:
      // _windowConfiguration->onScreenDisplay = !_windowConfiguration->onScreenDisplay;
      // break;
      // case YarsKeyFunction::ToggleTextures:
      // _windowConfiguration->useTextures = !_windowConfiguration->useTextures;
      // break;
    case YarsKeyFunction::ToggleFollowMode:
      __toggleFollowing();
      break;
    case YarsKeyFunction::CloseWindow:
      _closed = true;
      notifyObservers(_m_closeWindow);
      break;
      // case YarsKeyFunction::ToggleTraces:
      // _windowConfiguration->useTraces = !_windowConfiguration->useTraces;
      // break;
    case YarsKeyFunction::PreviousFollowable:
      __previousFollowable();
      break;
    case YarsKeyFunction::NextFollowable:
      __nextFollowable();
      break;
    case YarsKeyFunction::PreviousFollowMode:
      __previousFollowMode();
      break;
    case YarsKeyFunction::NextFollowMode:
      __nextFollowMode();
      break;
  }
}

void SdlWindow::__previousFollowable()
{
  _windowConfiguration->useFollow = true;
  int nr = Data::instance()->current()->screens()->followables()->size() - 1;
  _followableIndex--;
  if(_followableIndex < 0) _followableIndex = nr;
  _followableObject = GET_FOLLOWABLE(_followableIndex);
  _cameraMan->follow(_followableObject);
}

void SdlWindow::__nextFollowable()
{
  _windowConfiguration->useFollow = true;
  int nr = Data::instance()->current()->screens()->followables()->size();
  _followableIndex = (_followableIndex + 1) % nr;
  _followableObject = GET_FOLLOWABLE(_followableIndex);
  _cameraMan->follow(_followableObject);
}


void SdlWindow::__previousFollowMode()
{
  _cameraMan->previousFollowMode();
  if(_windowConfiguration->useFollow)
  {
    _cameraMan->follow(_followableObject);
  }
}

void SdlWindow::__nextFollowMode()
{
  _cameraMan->nextFollowMode();
  if(_windowConfiguration->useFollow)
  {
    _cameraMan->follow(_followableObject);
  }
}

void SdlWindow::__toggleFollowing()
{
  CHECK_IF_THERE_ARE_FOLLOWABLES;
  _windowConfiguration->useFollow = !_windowConfiguration->useFollow;
  if(_followableIndex == -1) _followableIndex = 0;
  _followableObject = GET_FOLLOWABLE(_followableIndex);
  if(_windowConfiguration->useFollow) _cameraMan->follow(_followableObject);
}

// void SdlWindow::__openNewWindow()
// {
// notifyObservers(_m_newWindow);
// }

void SdlWindow::__toggleWriteFrames()
{
  _imgCaptureRunning = !_imgCaptureRunning;
  notifyObservers(_m_toggleSyncedGui);
  if(_imgCaptureRunning) __YARS_OPEN_FRAMES_DIRECTORY;
}

#ifdef USE_CAPTURE_VIDEO
void SdlWindow::__toggleCaptureMovie()
{
  _captureRunning = !_captureRunning;
  notifyObservers(_m_toggleSyncedGui);
  if(_captureRunning)
  {
    __initMovie();
  }
  else
  {
    __closeMovie();
  }
}
#endif // USE_CAPTURE_VIDEO

#ifdef USE_CAPTURE_VIDEO
void SdlWindow::__initMovie()
{
  _windowConfiguration->getNextCaptureName();
  std::stringstream oss;
  __YARS_OPEN_CAPTURE_DIRECTORY;
  _captureStep = __YARS_GET_SIMULATOR_FREQUENCY / __YARS_GET_CAPTURE_FRAME_RATE;
  _capturedTenMinutes = 600 * __YARS_GET_CAPTURE_FRAME_RATE;
  uint width  = _viewport->getActualWidth();
  uint height = _viewport->getActualHeight();
  oss << __YARS_GET_CAPTURE_DIRECTORY << "/" << _windowConfiguration->captureName;

  // _avCodec = avcodec_find_encoder_by_name("avc1");
  // _avCodec = avcodec_find_encoder_by_name(__YARS_GET_VIDEO_CODEC.c_str());
  _avCodec = avcodec_find_encoder(AV_CODEC_ID_H264);
  if (!_avCodec) {
    YarsErrorHandler::push("Video codec %s not found.", __YARS_GET_VIDEO_CODEC.c_str());
    exit(0);
  }

  _avContext = avcodec_alloc_context3(_avCodec);

  cout << "capturing video with width " << width
    << " and height " << height << " and codec " << __YARS_GET_VIDEO_CODEC.c_str() << " \n";
  cout << "Starting video capture of " << oss.str().c_str() << " with frame rate "
    << __YARS_GET_CAPTURE_FRAME_RATE <<  "." << endl;

  /* put sample parameters */
  _avContext->bit_rate = 400000;
  _avContext->width = width;
  _avContext->height = height;
  _avContext->time_base = (AVRational){1, __YARS_GET_CAPTURE_FRAME_RATE};
  _avContext->framerate = (AVRational){__YARS_GET_CAPTURE_FRAME_RATE, 1};
  _avContext->gop_size = 10;
  _avContext->max_b_frames = 1;
  _avContext->pix_fmt = AV_PIX_FMT_YUV420P;

  if (_avCodec->id == AV_CODEC_ID_H264)
    av_opt_set(_avContext->priv_data, "preset", "slow", 0);

  /* open it */
  int ret = avcodec_open2(_avContext, _avCodec, NULL);
  if (ret < 0) {
    fprintf(stderr, "Could not open codec: %s\n", av_err2str(ret));
    exit(1);
  }

  _videoFileHandler = fopen(oss.str().c_str(), "wb");
  if (!_videoFileHandler) {
    fprintf(stderr, "could not open %s\n", oss.str().c_str());
    exit(1);
  }

  _avFrame = av_frame_alloc();
  if (!_avFrame)
  {
    fprintf(stderr, "Could not allocate video frame\n");
    exit(1);
  }

  _avFrame->format = _avContext->pix_fmt;
  _avFrame->width  = _avContext->width;
  _avFrame->height = _avContext->height;
  ret = av_frame_get_buffer(_avFrame, 32);
  if (ret < 0) {
    fprintf(stderr, "Could not allocate the video frame data\n");
    exit(1);
  }

  _avPkt = av_packet_alloc();
  if(!_avPkt)
  {
    fprintf(stderr, "Could not allocate video packet\n");
    exit(1);
  }


  _captureRunning = true;
  _capturingOffset = __YARS_GET_STEP;
  _frameIndex = 0;

  if(_swsContext != NULL)
  {
    // delete _swsContext;
  }

  _swsContext = sws_getContext(width, height,
                               AV_PIX_FMT_RGB32,
                               _avContext->width,
                               _avContext->height,
                               AV_PIX_FMT_YUV420P,
                               0, NULL, NULL, NULL);

  __initRenderFrame();
}

void SdlWindow::__closeMovie()
{
  Ogre::TextureManager::getSingleton().remove("StreamTex");
  _captureRunning = false;
  uint8_t endcode[] = { 0, 0, 1, 0xb7 };
  fwrite(endcode, 1, sizeof(endcode), _videoFileHandler);
  fclose(_videoFileHandler);
  avcodec_free_context(&_avContext);
  av_frame_free(&_avFrame);
  av_packet_free(&_avPkt);
}

void SdlWindow::__captureMovieFrame()
{
  if(__YARS_GET_STEP % _captureStep != 0) return;
  if(__YARS_GET_USE_PAUSE) return;
  if(_frameIndex > 0 && _frameIndex % _capturedTenMinutes == 0)
  {
    __closeMovie();
    __initMovie();
  }
  int ret, got_output;
  _frameIndex++;
  uint width  = _viewport->getActualWidth();
  uint height = _viewport->getActualHeight();

  ConsoleView::printCapturingInformation(_frameIndex);

  _pRenderTex = _renderTexture->getBuffer()->getRenderTarget();
  _pRenderTex->update();
  Ogre::HardwarePixelBufferSharedPtr buffer = _renderTexture->getBuffer();
  uint8_t *pData = (uint8_t*)buffer->lock(0, width*height*4, Ogre::HardwareBuffer::HBL_READ_ONLY);

  const int in_linesize[1] = { 4 * _avContext->width };
  _swsContext = sws_getCachedContext(_swsContext,
                                     width, height, AV_PIX_FMT_RGB32,
                                     _avContext->width, _avContext->height, AV_PIX_FMT_YUV420P,
                                     0, NULL, NULL, NULL);
  sws_scale(_swsContext, (const uint8_t * const *)&pData, in_linesize, 0,
            _avContext->height, _avFrame->data, _avFrame->linesize);

  av_init_packet(_avPkt);
  _avPkt->data = NULL;
  _avPkt->size = 0;
  _avFrame->pts = (1.0 / 30.0) * 90.0 * _frameIndex;

  ret = avcodec_encode_video2(_avContext, _avPkt, _avFrame, &got_output);
  if (ret < 0) {
    fprintf(stderr, "Error encoding _avFrame\n");
    exit(1);
  }
  if (got_output) {
    fwrite(_avPkt->data, 1, _avPkt->size, _videoFileHandler);
    av_packet_unref(_avPkt);
  }

}

#endif // USE_CAPTURE_VIDEO

void SdlWindow::__initRenderFrame()
{
  stringstream oss;
  oss << "render texture " << _index;
  if(!Ogre::TextureManager::getSingleton().resourceExists(oss.str()))
  {
    _renderTexture = Ogre::TextureManager::getSingleton().createManual(oss.str(),
                                                                       Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                                       Ogre::TEX_TYPE_2D,
                                                                       _viewport->getActualWidth(),
                                                                       _viewport->getActualHeight(),
                                                                       0,
                                                                       Ogre::PF_B8G8R8A8,
                                                                       Ogre::TU_RENDERTARGET);

    _pRenderTex = _renderTexture->getBuffer()->getRenderTarget();
    _pRenderTex->addViewport(_camera);

    Ogre::Viewport *vp = _pRenderTex->getViewport(0);
    vp->setClearEveryFrame(true);
    vp->setBackgroundColour(Ogre::ColourValue::Black);
    vp->setOverlaysEnabled(true);
  }
}

void SdlWindow::__captureImageFrame()
{
  __initRenderFrame();
  ConsoleView::printCapturingInformation(_imgCaptureFrameIndex);
  _imgCaptureFrameIndex++;
  stringstream oss;
  oss << __YARS_GET_FRAMES_DIRECTORY << "/frame_" << setfill('0') << setw(8)
    << _imgCaptureFrameIndex       << ".png";
  _pRenderTex = _renderTexture->getBuffer()->getRenderTarget();
  _pRenderTex->update();
  _pRenderTex->writeContentsToFile(oss.str());
}

void SdlWindow::captureVideo()
{
#ifdef USE_CAPTURE_VIDEO
  if(_captureRunning)    __captureMovieFrame();
#endif // USE_CAPTURE_VIDEO
  if(_imgCaptureRunning) __captureImageFrame();
}

#ifdef USE_CAPTURE_VIDEO
bool SdlWindow::captureRunning()
{
  return _captureRunning;
}
#endif // USE_CAPTURE_VIDEO

void SdlWindow::__osd()
{
  if(_windowConfiguration->osdElapsedTime)
  {
    if(_index == 0) _textOverlay->setText(_timeString, OSD::getElapsedTimeString());
  }
  if( _windowConfiguration->osdRobotInformation)
  {
    std::stringstream oss;
    DataRobots *robots = Data::instance()->current()->robots();
    for(std::vector<DataRobot*>::iterator m = robots->begin(); m != robots->end(); m++)
    {
      DataController *controller = (*m)->controller();
      if(controller != NULL)
      {
        controller->lockOSD();
        for(std::vector<string>::const_iterator s = controller->s_begin(); s != controller->s_end(); s++)
        {
          oss << *s;
        }
        controller->unlockOSD();
      }
    }
    if(_index == 0) _textOverlay->setText(_robotString, oss.str(), (int)_viewport->getActualHeight());
  }
}

void SdlWindow::toggleShadows()
{
  _shadowMode = _nextShadowMode % (__SHADOWTYPE_NONE + 1);
  switch(_nextShadowMode)
  {
    case __SHADOWTYPE_STENCIL_ADDITIVE:
      _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
      Y_INFO("Shadow type stencil additive");
      break;
    case __SHADOWTYPE_STENCIL_MODULATIVE:
      _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
      Y_INFO("Shadow type stencil modulative");
      break;
    case __SHADOWTYPE_TEXTURE_ADDITIVE:
      _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);
      _sceneManager->setShadowTextureSize(10240);
      _sceneManager->setShadowTextureCount(1);
      Y_INFO("Shadow type texture additive");
      break;
    case __SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED:
      _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
      _sceneManager->setShadowTextureSize(10240);
      _sceneManager->setShadowTextureCount(1);
      Y_INFO("Shadow type texture additive integrated");
      break;
    case __SHADOWTYPE_TEXTURE_MODULATIVE:
      _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
      _sceneManager->setShadowTextureSize(10240);
      _sceneManager->setShadowTextureCount(1);
      Y_INFO("Shadow type texture modulative");
      break;
    case __SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED:
      _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
      _sceneManager->setShadowTextureSize(10240);
      _sceneManager->setShadowTextureCount(1);
      Y_INFO("Shadow type texture modulative integrated");
      break;
    case __SHADOWTYPE_NONE:
      _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
      Y_INFO("Shadow type texture none");
      break;
  }
  _nextShadowMode = (_nextShadowMode + 1) % (__SHADOWTYPE_NONE + 1);
}


#ifdef USE_CAPTURE_VIDEO
void SdlWindow::startCaptureVideo()
{
  _captureRunning = true;
  __initMovie();
}

void SdlWindow::stopCaptureVideo()
{
  _captureRunning = false;
  __closeMovie();
}
#endif // USE_CAPTURE_VIDEO

bool SdlWindow::visible()
{
  return _visible;
}

void SdlWindow::__handleFingerUp(SDL_Event &event)
{
  //Rotation detected
  // if( fabs( event.mgesture.dTheta ) > 3.14 / 180.0 )
  // {
  // cout << "Rotation detected: " << event.mgesture.dTheta << endl;
  // }
  // if( fabs( event.mgesture.dDist ) > 0.002 )
  // {
  // cout << "Zoom detected: " << event.mgesture.dDist << endl;
  // }
}

void SdlWindow::__handleFingerDown(SDL_Event &event)
{
  //Rotation detected
  // if( fabs( event.mgesture.dTheta ) > 3.14 / 180.0 )
  // {
  // cout << "Rotation detected: " << event.mgesture.dTheta << endl;
  // }
  // if( fabs( event.mgesture.dDist ) > 0.002 )
  // {
  // cout << "Zoom detected: " << event.mgesture.dDist << endl;
  // }
}

void SdlWindow::setAdded()
{
  _added = true;
}

bool SdlWindow::added()
{
  return _added;
}

bool SdlWindow::closed()
{
  return _closed;
}

void SdlWindow::close()
{
  _pRenderTex    = NULL; // might have to be removed from ogre first
  // &_renderTexture = NULL; // might have to removed from ogre first

  _window        = NULL; // might have to be removed from ogre first
  _camera        = NULL; // might have to be removed from ogre first
  _viewport      = NULL; // might have to be removed from ogre first
  _sceneManager  = NULL; // might have to be removed from ogre first
  _ogreHandler   = NULL; // might have to be removed from ogre first

  SDL_DestroyWindow(_sdlWindow);
}


