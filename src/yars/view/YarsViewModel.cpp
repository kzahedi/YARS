#include "YarsViewModel.h"

#include <yars/configuration/data/Data.h>
#include <yars/configuration/YarsConfiguration.h>
#include <yars/view/gui/GuiMutex.h>
#include <yars/util/Timer.h>

#include <OGRE/Ogre.h>

namespace yars {

YarsViewModel::YarsViewModel()
{
  _drawFequency = 1;
  _visualiseCalled = 0;
  _run = true;
  _sync = false;
  _syncedStep = false;
  _toggleVideo = false;
  _timeStamp = 0;
  _first = -1;
  _last = 0;
  _ogreHandler = NULL;

  if (__YARS_GET_USE_VISUALISATION)
  {
    // Check if we're in a headless environment (Linux/X11 only)
#ifdef __linux__
    const char *display = getenv("DISPLAY");
    bool hasDisplay = (display != nullptr && strlen(display) > 0);

    if (!hasDisplay)
    {
      std::cout << "No display detected - automatically switching to headless mode." << std::endl;
      std::cout << "Physics simulation will run without visualization." << std::endl;
      // Don't initialize GUI components, just continue with physics
      return;
    }
#endif
    // On macOS and Windows, assume GUI is available and let initialization handle any failures

    try
    {
      _ogreHandler = OgreHandler::instance();
      initialiseView();
      _ogreHandler->setupSceneManager();
      FOREACH(SdlWindow *, i, _windowManager)
      if ((*i) != NULL)
        (*i)->setupOSD();
      if (__YARS_GET_USE_CAPTURE_CL)
        toggleCaptureVideo();
    }
    catch (const Ogre::RenderingAPIException &e)
    {
      std::cerr << std::endl;
      std::cerr << "===============================================" << std::endl;
      std::cerr << "GUI INITIALIZATION FAILED - CONTINUING HEADLESS" << std::endl;
      std::cerr << "===============================================" << std::endl;
      std::cerr << "OpenGL Error: " << e.what() << std::endl;
      std::cerr << std::endl;
      std::cerr << "Automatically switching to headless mode." << std::endl;
      std::cerr << "Physics simulation will continue without visualization." << std::endl;
      std::cerr << std::endl;
      std::cerr << "To avoid this message in the future, use: --nogui" << std::endl;
      std::cerr << "===============================================" << std::endl;
      // Reset to NULL and continue without GUI
      _ogreHandler = NULL;
    }
    catch (const std::exception &e)
    {
      std::cerr << std::endl;
      std::cerr << "GUI initialization failed: " << e.what() << std::endl;
      std::cerr << "Continuing in headless mode." << std::endl;
      std::cerr << "Use --nogui flag to avoid this message." << std::endl;
      // Reset to NULL and continue without GUI
      _ogreHandler = NULL;
    }
  }
}

YarsViewModel::~YarsViewModel()
{
  Y_DEBUG("YarsViewModel destructor called.");
}

void YarsViewModel::initialiseView()
{
  DataRobotSimulationDescription *data = __YARS_CURRENT_DATA;
  if (!__YARS_GET_USE_VISUALISATION)
    return;
  if (data->screens() == NULL)
    return;
  FOREACHP(DataScreen *, i, data->screens())
  if ((*i)->autoShow())
    __createWindow();
}

void YarsViewModel::visualiseScene()
{
  if (!__YARS_GET_USE_VISUALISATION)
    return;
  if (__YARS_CURRENT_DATA->screens() == NULL)
    return;
  if (_ogreHandler == NULL)
    return; // Skip visualization if GUI failed to initialize

  _ogreHandler->step();

  FOREACH(SdlWindow *, i, _windowManager)
  if ((*i) != NULL)
    (*i)->step();
  while (SDL_PollEvent(&_event))
  {
    FOREACH(SdlWindow *, i, _windowManager)
    if ((*i) != NULL)
      (*i)->handleEvent(_event);
  }
}

void YarsViewModel::reset()
{
  if (_ogreHandler != NULL)
    _ogreHandler->reset();
  FOREACH(SdlWindow *, i, _windowManager)
  (*i)->reset();
}

void YarsViewModel::quit()
{
  Y_DEBUG("YarsViewModel::quit called")
  _run = false;
  // SDL cleanup will be handled by main thread when run() loop exits
  Y_DEBUG("YarsViewModel::quit completed")
}

void YarsViewModel::__createWindow()
{
  SdlWindow *wm = new SdlWindow(_windowManager.size());
  wm->addObserver(this);
  _windowManager.push_back(wm);
}

void YarsViewModel::createNewWindow()
{
  if (_ogreHandler == NULL)
    return; // Can't create windows without GUI

  SdlWindow *wm = new SdlWindow(_windowManager.size() + _newWindows.size());
  wm->addObserver(this);
#ifdef USE_CAPTURE_VIDEO
  if (wm->captureRunning())
  {
    _sync = true;
    _syncedStep = true;
  }
#endif // USE_CAPTURE_VIDEO
  wm->wait();
  _ogreHandler->step();
  wm->step();
  _newWindows.push_back(wm);
  _timeStamp = Timer::getTime();
  _first++;
}

// Observer pattern methods replaced with direct method calls
// quit() method already handles window cleanup

void YarsViewModel::cleanupWindows()
{
  vector<SdlWindow *> toBeDeleted;
  for (std::vector<SdlWindow *>::iterator i = _windowManager.begin(); i != _windowManager.end(); i++)
  {
    if ((*i)->closed())
    {
      toBeDeleted.push_back(*i);
    }
  }

  for (std::vector<SdlWindow *>::iterator i = toBeDeleted.begin(); i != toBeDeleted.end(); i++)
  {
    (*i)->close();
    _windowManager.erase(i);
    // _newWindows.erase(i);
  }
}

void YarsViewModel::run()
{
  while (_run)
  {
    if (_sync)
    {
      if (_syncedStep)
      {
        visualiseScene();
#ifdef USE_CAPTURE_VIDEO
        if (_toggleVideo == true)
        {
          FOREACH(SdlWindow *, i, _windowManager)
          (*i)->captureVideo();
        }
#endif // USE_CAPTURE_VIDEO
        _syncedStep = false;
      }
    }
    else
    {
      visualiseScene();
    }

    for (std::vector<SdlWindow *>::iterator i = _newWindows.begin(); i != _newWindows.end(); i++)
    {
      if ((*i)->added() == false)
      {
        _windowManager.push_back(*i);
        (*i)->setAdded();
      }
    }
  }

  // Cleanup SDL resources on main thread after main loop exits
  Y_DEBUG("YarsViewModel::run() main loop exited, cleaning up SDL resources on main thread")
  FOREACH(SdlWindow *, i, _windowManager)
  (*i)->close();
  _windowManager.clear();
  Y_DEBUG("YarsViewModel::run() SDL cleanup completed")
}

void YarsViewModel::synched()
{
  if (!_sync)
    return;
  _syncedStep = true;
  while (_syncedStep)
    usleep(1000);
}

void YarsViewModel::toggleShadows()
{
  FOREACH(SdlWindow *, i, _windowManager)
  (*i)->toggleShadows();
}

void YarsViewModel::toggleCaptureVideo()
{
  _sync = !_sync;
  _toggleVideo = !_toggleVideo;

#if USE_CAPTURE_VIDEO
  if (_toggleVideo == true)
  {
    FOREACH(SdlWindow *, i, _windowManager)
    (*i)->startCaptureVideo();
  }
  else
  {
    cout << "stopped video recording" << endl;
    FOREACH(SdlWindow *, i, _windowManager)
    (*i)->stopCaptureVideo();
  }
#endif // USE_CAPTURE_VIDEO
}

} // namespace yars
