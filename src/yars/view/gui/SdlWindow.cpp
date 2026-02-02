#include "SdlWindow.h"

#include <yars/configuration/data/Data.h>
#include <yars/configuration/YarsConfiguration.h>
#include <yars/view/gui/KeyHandler.h>
#include <yars/view/console/ConsoleView.h>
#include <yars/util/macros.h>
#include <yars/util/OSD.h>

#include <OGRE/Ogre.h>
#include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>

#ifndef __APPLE__
namespace _SDL_
{
#endif // __APPLE__
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_thread.h>
#ifndef __APPLE__
};

using namespace _SDL_;
#endif // __APPLE__

#include <math.h>
#include <string>

#define FACTOR 0.01

#define __NO_KEY 0
#define __CONTROL 1
#define __ALT 2
#define __SHIFT 4

#define __SHADOWTYPE_STENCIL_ADDITIVE 0
#define __SHADOWTYPE_STENCIL_MODULATIVE 1
#define __SHADOWTYPE_TEXTURE_ADDITIVE 2
#define __SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED 3
#define __SHADOWTYPE_TEXTURE_MODULATIVE 4
#define __SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED 5
#define __SHADOWTYPE_NONE 6

#define CHECK_IF_THERE_ARE_FOLLOWABLES                                      \
  if (Data::instance()->current()->screens()->followables() == nullptr)        \
    return;                                                                 \
  if (Data::instance()->current()->screens()->followables()->o_size() == 0) \
    return;

#define GET_FOLLOWABLE(a) \
  Data::instance()->current()->screens()->followables()->followable(a)

// bad SDL, bad.. seems to be Windows only at least
#ifdef main
#undef main
#endif

// Threading now uses C++17 std::thread

#include <string>

#ifdef __APPLE__
#include "OSX_wrap.h"
#endif

using namespace std;

SdlWindow::SdlWindow(int index)
    : _visible(false),
      _added(false)
{
  _index = index;
  _windowConfiguration = new WindowConfiguration(index);
  _cameraMan = new CameraMan(_windowConfiguration);
  _data = Data::instance()->current()->screens()->screen(index);
  _camData = Data::instance()->current()->screens()->screen(index)->camera();
  _mousePressed = false;
  _shiftPressed = false;
  _ctrlPressed = false;
  _altPressed = false;
  _metaPressed = false;
  _imgCaptureRunning = false;
  _imgCaptureFrameIndex = 0;
  _followableIndex = 0;
  _closed = false;
  _fps = 0;
  _lastTime = 0;
  _currentTime = 0;
  _lastStep = 0;
  _shadowMode = __SHADOWTYPE_TEXTURE_ADDITIVE;
  _nextShadowMode = __SHADOWTYPE_TEXTURE_ADDITIVE + 1;
  _cameraVelocity = Ogre::Vector3::ZERO;
  _camAngularVelocity = 0.0;
  __setupSDL();
  // __setScene();

  if (_windowConfiguration->useFollow) // start with following
  {
    _windowConfiguration->useFollow = false; // is set to true in __toggleFollowing
    __toggleFollowing();
  }

#ifdef USE_CAPTURE_VIDEO
  _captureRunning = false;
  _videoCapture = nullptr;
#endif // USE_CAPTURE_VIDEO
}

void SdlWindow::wait()
{
  SDL_Event event;
  int timeout = 100;  // Max ~1 second wait (100 * 10ms)

  while (!_visible && timeout > 0)
  {
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_SHOWN)
      {
        _visible = true;
        return;
      }
    }
    usleep(10000);  // 10ms
    timeout--;
  }

  // If we didn't get the SHOWN event, assume window is ready after timeout
  _visible = true;
}

void SdlWindow::swapBuffers()
{
#ifdef __APPLE__
  // macOS with externalGLControl: Use SDL to swap buffers
  if (_glContext != nullptr && _sdlWindow != nullptr)
  {
    SDL_GL_SwapWindow(_sdlWindow);
  }
#else
  // Other platforms: Let OGRE handle buffer swap
  if (_window != nullptr)
  {
    _window->swapBuffers();
  }
#endif
}

void SdlWindow::step()
{

#ifdef USE_CAPTURE_VIDEO
  if (_captureRunning || _imgCaptureRunning)
#else  // USE_CAPTURE_VIDEO
  if (_imgCaptureRunning)
#endif // USE_CAPTURE_VIDEO
  {
    if (_index == 0)
      _textOverlay->setText(_statsString, "");
  }
  else
  {
    _fps++;
    _currentTime = SDL_GetTicks();
    if (_currentTime > _lastTime + 500)
    {
      int step = __YARS_GET_CONTINUOUS_STEP;

      double rt = (double)(step - _lastStep) / ((double)__YARS_GET_SIMULATOR_FREQUENCY);
      // double f    = (double)_fps;
      double diff = 1000.0 / ((double)(_currentTime - _lastTime));

      // f  =  f * diff;
      rt = rt * diff;
      _fpsString.str("");

      _fpsString << std::fixed << std::setprecision(2) << rt << " RT\n";
      // _fpsString << std::fixed << std::setprecision(2) << f << " FPS";
      if (_index == 0)
        _textOverlay->setText(_statsString, _fpsString.str());
      _lastTime = _currentTime;
      _lastStep = step;
      _fps = 0;
    }
  }

  if (_windowConfiguration->useFollow)
  {
    // OGRE 14: Use camera node for position/direction
    _cpos = _cameraNode->getPosition();
    _cdir = _cameraNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
    _clookAt = _cpos;
    for (int i = 0; i < 3; i++)
      _clookAt[i] += _cdir[i];
    OGRE_TO_YARS(_cpos, _ypos);
    OGRE_TO_YARS(_clookAt, _ylookAt);
    _camData->setPosition(_ypos);
    _camData->setLookAt(_ylookAt);
    OGRE_TO_YARS(_cameraVelocity, _camVelocity);

    _cameraMan->update(_camVelocity);

    YARS_TO_OGRE(_camData->position(), _cpos);
    YARS_TO_OGRE(_camData->lookAt(), _clookAt);
    _cameraNode->setPosition(_cpos[0], _cpos[1], _cpos[2]);
    _cameraNode->lookAt(Ogre::Vector3(_clookAt[0], _clookAt[1], _clookAt[2]), Ogre::Node::TS_WORLD);
  }
  else if (_cameraVelocity.length() > 0.01 ||
           _camAngularVelocity.length() > 0.0001)
  {
    // OGRE 14: Use camera node for yaw/pitch/move
    _cameraNode->yaw(Ogre::Radian(_camAngularVelocity.x * FACTOR));
    _cameraNode->pitch(Ogre::Radian(_camAngularVelocity.y * FACTOR));

    _cameraNode->translate(_cameraVelocity, Ogre::Node::TS_LOCAL);

    _cpos = _cameraNode->getPosition();
    _cdir = _cameraNode->getOrientation() * Ogre::Vector3::NEGATIVE_UNIT_Z;
    _clookAt = _cpos;
    for (int i = 0; i < 3; i++)
      _clookAt[i] += _cdir[i];
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
  if (event.window.windowID != _windowID)
    return;

  switch (event.type)
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
    _ctrlPressed = false;
    _altPressed = false;
    _metaPressed = false;
    break;

  case SDL_MOUSEMOTION:
    if (_mousePressed)
    {
      if (_metaPressed && !_altPressed)
      {
        _cameraVelocity[0] += -event.motion.xrel * FACTOR;
        _cameraVelocity[2] += -event.motion.yrel * FACTOR;
      }
      if (!_metaPressed && _altPressed)
      {
        _cameraVelocity[1] += event.motion.yrel * FACTOR;
      }
      if (!_metaPressed && !_altPressed)
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
      if (_closeCallback)
        _closeCallback();
      break;
    case SDL_WINDOWEVENT_RESIZED:
      _window->resize(event.window.data1, event.window.data2);
      _window->windowMovedOrResized();
      const Ogre::Real aspectRatio = Ogre::Real(_viewport->getActualWidth()) / Ogre::Real(_viewport->getActualHeight());
      _camera->setAspectRatio(aspectRatio);

      Ogre::Real x = _viewport->getActualWidth() - 140;
      Ogre::Real y = 10;
      if (_index == 0)
        _textOverlay->setPosition(_legendString, x, y);
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
  SDL_Init(SDL_INIT_EVERYTHING);
  // SDL_Init(SDL_INIT_VIDEO);

  // Create SDL window with OpenGL support
  Uint32 windowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;

#ifdef __APPLE__
  // macOS: Request OpenGL 4.1 Core Profile (highest supported on macOS, matches OGRE GL3Plus)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
  // Use sRGB framebuffer
  SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
#endif

  if (__YARS_GET_USE_WINDOW_GEOMETRY)
  {
    _sdlWindow = SDL_CreateWindow(_windowConfiguration->name.c_str(),
                                  _windowConfiguration->geometry.x(),
                                  _windowConfiguration->geometry.y(),
                                  _windowConfiguration->geometry.width(),
                                  _windowConfiguration->geometry.height(),
                                  windowFlags);
  }
  else
  {
    _sdlWindow = SDL_CreateWindow(_windowConfiguration->name.c_str(),
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  _windowConfiguration->geometry.width(),
                                  _windowConfiguration->geometry.height(),
                                  windowFlags);
  }

  if (_sdlWindow == nullptr)
  {
    printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
    return;
  }

  // SDL_WarpMouse(800/2, 600/2);
  // SDL_WM_GrabInput(SDL_GRAB_OFF);
  // SDL_ShowCursor(SDL_ENABLE);

  // Create GL context for platforms that need explicit context management
  _glContext = nullptr;
#if defined(__WINDOWS__) || defined(__APPLE__)
  _glContext = SDL_GL_CreateContext(_sdlWindow);
  if (_glContext == nullptr)
  {
    printf("SDL_GL_CreateContext failed: %s\n", SDL_GetError());
    return;
  }
  // Make context current before OGRE uses it
  SDL_GL_MakeCurrent(_sdlWindow, _glContext);
#endif

  SDL_SysWMinfo syswm_info;
  SDL_VERSION(&syswm_info.version);
  if (!SDL_GetWindowWMInfo(_sdlWindow, &syswm_info))
  {
    printf("SDL_GetWindowWMInfo failed.\n");
    return;
  }

  // Disable vsync - let the application control timing
  SDL_GL_SetSwapInterval(0);

  Ogre::NameValuePairList params;
#ifdef __WINDOWS__
  params["externalGLControl"] = "1";
  // only supported for Win32 on Ogre 1.8 not on other platforms (documentation needs fixing to accurately reflect this)
  params["externalGLContext"] = Ogre::StringConverter::toString((unsigned long)_glContext);
  params["externalWindowHandle"] = Ogre::StringConverter::toString((unsigned long)syswm_info.info.win.window);
#endif
#ifdef __linux__
  // params["externalGLControl"] = "1";
  // params["currentGLContext"] = "1";
  params["parentWindowHandle"] = Ogre::StringConverter::toString((unsigned long)syswm_info.info.x11.window);
#endif
#ifdef __APPLE__
  // OGRE 14 + SDL2 on macOS: Pass SDL's GL context and NSView to OGRE
  // SDL_GLContext on macOS is actually an NSOpenGLContext pointer
  params["externalWindowHandle"] = OSX_cocoa_view(syswm_info);
  params["externalGLContext"] = Ogre::StringConverter::toString((size_t)_glContext);
  params["externalGLControl"] = "true";
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
  _window->setVisible(true);

  Ogre::Vector3 pos;
  Ogre::Vector3 lookAt;

  YARS_TO_OGRE(_windowConfiguration->cameraPosition, pos);
  YARS_TO_OGRE(_windowConfiguration->cameraLookAt, lookAt);

  _sceneManager = _ogreHandler->getSceneManager();
  oss.str("");
  oss << "YARS Camera" << _index;
  _camera = _sceneManager->createCamera(oss.str());
  _camera->setNearClipDistance(0.01f);
  _camera->setFarClipDistance(1000000.0f);

  // OGRE 14: Create camera node and attach camera
  oss.str("");
  oss << "YARS CameraNode" << _index;
  _cameraNode = _sceneManager->getRootSceneNode()->createChildSceneNode(oss.str());
  _cameraNode->attachObject(_camera);
  _cameraNode->setPosition(pos[0], pos[1], pos[2]);
  _cameraNode->lookAt(Ogre::Vector3(lookAt[0], lookAt[1], lookAt[2]), Ogre::Node::TS_WORLD);

  const Ogre::Real aspectRatio = Ogre::Real(_windowConfiguration->geometry.width()) / Ogre::Real(_windowConfiguration->geometry.height());
  _camera->setAspectRatio(aspectRatio);

  _viewport = _window->addViewport(_camera);
  Ogre::ColourValue fadeColour(0.9, 0.9, 0.9);
  // _sceneManager->setFog(Ogre::FOG_EXP2, fadeColour, 0.001, 500.0, 1000.0);
  _viewport->setBackgroundColour(fadeColour);
  // OGRE 14: Set viewport to use RTSS scheme for shader generation
  _viewport->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

  _windowID = SDL_GetWindowID(_sdlWindow);

  SDL_RecordGesture(-1);
  _window->setActive(true);
  _viewport->update();
  _window->setActive(true);
  _window->setVisible(true);

  // Initial buffer swap
#ifdef __APPLE__
  SDL_GL_SwapWindow(_sdlWindow);
#else
  _window->swapBuffers();
#endif
}

void SdlWindow::setupOSD()
{
  if (_index != 0)
    return;

  Colour osdColour = _data->osdTimeFontColour();
  string osdFont = _data->osdTimeFontName();
  string fontsize;

  stringstream oss;
  oss << "text overlay " << _index;
  if (_index == 0)
    _textOverlay = new TextOverlay(oss.str());

  oss.str("");
  oss << _data->osdTimeFontSize();
  fontsize = oss.str();
  oss.str("");
  oss << "time " << _index;
  _timeString = oss.str();
  if (_index == 0)
    _textOverlay->addTextBox(_timeString, "00d:00h:00m:00s", 10, 10, 100, 20,
                             Ogre::ColourValue(osdColour.red(), osdColour.green(), osdColour.blue(), osdColour.alpha()),
                             osdFont, fontsize);

  oss.str("");
  oss << "stats " << _index;
  _statsString = oss.str();
  if (_index == 0)
    _textOverlay->addTextBox(_statsString, "", 10, 40, 100, 20,
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
  if (_index == 0)
    _textOverlay->addTextBox(_robotString, "", 10, _viewport->getActualHeight() - _data->osdRobotFontHeight() - 10,
                             _data->osdRobotFontWidth(), _data->osdRobotFontHeight(),
                             Ogre::ColourValue(osdColour.red(), osdColour.green(), osdColour.blue(), osdColour.alpha()),
                             osdFont, fontsize);

  Ogre::Real x = _viewport->getActualWidth() - 140;
  Ogre::Real y = 10;
  oss.str("");
  oss << "legend " << _index;
  _legendString = oss.str();
  if (_index == 0)
    _textOverlay->addTextBox(_legendString,
                             "^0YARS, Zahedi", x, y, 15, 10,
                             Ogre::ColourValue(75.0 / 255.0, 117.0 / 255.0, 148.0 / 255.0, 1.0f),
                             "Legend", "24");
}

void SdlWindow::reset()
{
}

void SdlWindow::quit()
{
#ifdef USE_CAPTURE_VIDEO
  if (_captureRunning)
    __closeMovie();
#endif // USE_CAPTURE_VIDEO
  // SDL_DestroyWindow(_sdlWindow);
  SDL_Quit();
}

void SdlWindow::__processKeyEvent(char chr, int mod)
{
  bool shift = (mod & KMOD_LSHIFT) || (mod & KMOD_RSHIFT);
#ifdef __APPLE__
  bool cmd = (mod & KMOD_LGUI) || (mod & KMOD_RGUI);
#else  // __APPLE__
  bool cmd = (mod & KMOD_LCTRL) || (mod & KMOD_RCTRL);
#endif // __APPLE__
  bool alt = (mod & KMOD_LALT) || (mod & KMOD_RALT);

  switch (KeyHandler::instance()->handleKeyEvent(alt, cmd, shift, chr))
  {
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
    if (_closeCallback)
      _closeCallback();
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
  if (_followableIndex < 0)
    _followableIndex = nr;
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
  if (_windowConfiguration->useFollow)
  {
    _cameraMan->follow(_followableObject);
  }
}

void SdlWindow::__nextFollowMode()
{
  _cameraMan->nextFollowMode();
  if (_windowConfiguration->useFollow)
  {
    _cameraMan->follow(_followableObject);
  }
}

void SdlWindow::__toggleFollowing()
{
  CHECK_IF_THERE_ARE_FOLLOWABLES;
  _windowConfiguration->useFollow = !_windowConfiguration->useFollow;
  if (_followableIndex == -1)
    _followableIndex = 0;
  _followableObject = GET_FOLLOWABLE(_followableIndex);
  if (_windowConfiguration->useFollow)
    _cameraMan->follow(_followableObject);
}

void SdlWindow::__toggleWriteFrames()
{
  _imgCaptureRunning = !_imgCaptureRunning;
  if (_syncToggleCallback)
    _syncToggleCallback();
  if (_imgCaptureRunning)
    __YARS_OPEN_FRAMES_DIRECTORY;
}

#ifdef USE_CAPTURE_VIDEO
void SdlWindow::__toggleCaptureMovie()
{
  _captureRunning = !_captureRunning;
  if (_syncToggleCallback)
    _syncToggleCallback();
  if (_captureRunning)
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
  _capturedTenMinutes = 6000 * __YARS_GET_CAPTURE_FRAME_RATE;
  // _capturedTenMinutes = 20 * __YARS_GET_CAPTURE_FRAME_RATE;
  uint width = _viewport->getActualWidth();
  uint height = _viewport->getActualHeight();
  oss << __YARS_GET_CAPTURE_DIRECTORY << "/" << _windowConfiguration->captureName;
  string filename = oss.str();

  _captureRunning = true;
  _capturingOffset = __YARS_GET_STEP;
  _frameIndex = 0;

  if (_videoCapture != nullptr)
  {
    delete _videoCapture;
  }
  _videoCapture = new VideoCapture();
  _videoCapture->init(width, height, __YARS_GET_CAPTURE_FRAME_RATE, 400, filename);

  __initRenderFrame();
}

void SdlWindow::__closeMovie()
{
  // Clean up the render texture used for video capture
  stringstream oss;
  oss << "render texture " << _index;
  if (Ogre::TextureManager::getSingleton().resourceExists(oss.str()))
  {
    Ogre::TextureManager::getSingleton().remove(oss.str());
    _renderTexture.reset();
  }
  _captureRunning = false;
  _videoCapture->finish();
  delete _videoCapture;
  _videoCapture = nullptr;
}

void SdlWindow::__captureMovieFrame()
{
  if (__YARS_GET_STEP % _captureStep != 0)
    return;
  if (__YARS_GET_USE_PAUSE)
    return;
  if (_frameIndex > 0 && _frameIndex % _capturedTenMinutes == 0)
  {
    __closeMovie();
    __initMovie();
  }

  // Ensure render texture is initialized
  if (!_renderTexture)
  {
    Y_DEBUG("Warning: _renderTexture is null in __captureMovieFrame");
    return;
  }

  uint width = _viewport->getActualWidth();
  uint height = _viewport->getActualHeight();

  ConsoleView::printCapturingInformation(_frameIndex);

  _pRenderTex = _renderTexture->getBuffer()->getRenderTarget();
  _pRenderTex->update();
  Ogre::HardwarePixelBufferSharedPtr buffer = _renderTexture->getBuffer();
  uint8_t *pData = (uint8_t *)buffer->lock(0, width * height * 4, Ogre::HardwareBuffer::HBL_READ_ONLY);

  _videoCapture->addFrame(pData);

  buffer->unlock();
  _frameIndex++;
}

#endif // USE_CAPTURE_VIDEO

void SdlWindow::__initRenderFrame()
{
  stringstream oss;
  oss << "render texture " << _index;
  if (!Ogre::TextureManager::getSingleton().resourceExists(oss.str()))
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
      << _imgCaptureFrameIndex << ".png";
  _pRenderTex = _renderTexture->getBuffer()->getRenderTarget();
  _pRenderTex->update();
  _pRenderTex->writeContentsToFile(oss.str());
}

void SdlWindow::captureVideo()
{
#ifdef USE_CAPTURE_VIDEO
  if (_captureRunning)
    __captureMovieFrame();
#endif // USE_CAPTURE_VIDEO
  if (_imgCaptureRunning)
    __captureImageFrame();
}

#ifdef USE_CAPTURE_VIDEO
bool SdlWindow::captureRunning()
{
  return _captureRunning;
}
#endif // USE_CAPTURE_VIDEO

void SdlWindow::__osd()
{
  if (_windowConfiguration->osdElapsedTime)
  {
    if (_index == 0)
      _textOverlay->setText(_timeString, OSD::getElapsedTimeString());
  }
  if (_windowConfiguration->osdRobotInformation)
  {
    std::stringstream oss;
    DataRobots *robots = Data::instance()->current()->robots();
    for (auto* robot : *robots)
    {
      DataController *controller = robot->controller();
      if (controller != nullptr)
      {
        controller->lockOSD();
        for (auto s = controller->s_begin(); s != controller->s_end(); s++)
        {
          oss << *s;
        }
        controller->unlockOSD();
      }
    }
    if (_index == 0)
      _textOverlay->setText(_robotString, oss.str(), (int)_viewport->getActualHeight());
  }
}

void SdlWindow::toggleShadows()
{
  _shadowMode = _nextShadowMode % (__SHADOWTYPE_NONE + 1);
  switch (_nextShadowMode)
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

void SdlWindow::__handleFingerUp([[maybe_unused]] SDL_Event &event)
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

void SdlWindow::__handleFingerDown([[maybe_unused]] SDL_Event &event)
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
  _pRenderTex = nullptr; // might have to be removed from ogre first
  // &_renderTexture = nullptr; // might have to removed from ogre first

  _window = nullptr;       // might have to be removed from ogre first
  _camera = nullptr;       // might have to be removed from ogre first
  _viewport = nullptr;     // might have to be removed from ogre first
  _sceneManager = nullptr; // might have to be removed from ogre first
  _ogreHandler = nullptr;  // might have to be removed from ogre first

  SDL_DestroyWindow(_sdlWindow);
}
