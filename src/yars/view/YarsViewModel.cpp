#include "YarsViewModel.h"

#include <yars/configuration/data/Data.h>
#include <yars/configuration/YarsConfiguration.h>
#include <yars/view/gui/GuiMutex.h>
#include <yars/util/Timer.h>

#include <OGRE/Ogre.h>
#include <unistd.h>

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
      for (auto &i : _windowManager) if (i) i->setupOSD();
      if (__YARS_GET_USE_CAPTURE_CL || __YARS_GET_FRAMES_DIRECTORY.length() > 0)
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

void YarsViewModel::initialiseView()
{
  DataRobotSimulationDescription *data = __YARS_CURRENT_DATA;
  if (!__YARS_GET_USE_VISUALISATION)
    return;
  if (data->screens() == NULL)
    return;
  for (auto *i : *data->screens())
    if (i->autoShow())
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

  for (auto &i : _windowManager) if (i) i->step();

  while (SDL_PollEvent(&_event))
  {
    for (auto &i : _windowManager) if (i) i->handleEvent(_event);
  }
}

void YarsViewModel::reset()
{
  if (_ogreHandler != NULL)
    _ogreHandler->reset();
  for (auto &i : _windowManager) i->reset();
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
  _windowManager.emplace_back(std::make_unique<SdlWindow>(_windowManager.size()));
}

void YarsViewModel::createNewWindow()
{
  if (_ogreHandler == NULL)
    return; // Can't create windows without GUI

  auto wm = std::make_unique<SdlWindow>(_windowManager.size() + _newWindows.size());
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
  _newWindows.push_back(std::move(wm));
  _timeStamp = Timer::getTime();
  _first++;
}

// Observer pattern methods replaced with direct method calls
// quit() method already handles window cleanup

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
          for (auto &i : _windowManager) i->captureVideo();
        }
#endif // USE_CAPTURE_VIDEO
        _syncedStep = false;
      }
      else
      {
        usleep(500); // yield CPU while waiting for next physics step
      }
    }
    else
    {
      visualiseScene();
    }

    // Note: GUI does NOT auto-exit based on step count.
    // Physics thread calls _quitCallback() → quit() → _run=false after
    // completing its last synced step, preventing a race condition where
    // the GUI exits before physics finishes the final synched() handshake.

    // Move any newly-created windows over to the main manager. After
    // the move, _newWindows entries are null; clear them in one shot.
    for (auto &i : _newWindows)
    {
      if (i)
      {
        i->setAdded();
        _windowManager.push_back(std::move(i));
      }
    }
    _newWindows.clear();
  }

  // Cleanup SDL resources on main thread after main loop exits
  Y_DEBUG("YarsViewModel::run() main loop exited, cleaning up SDL resources on main thread")
  for (auto &i : _windowManager)
  {
    i->quit(); // finalize video capture before destroying window
    i->close();
  }
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
  for (auto &i : _windowManager) i->toggleShadows();
}

void YarsViewModel::toggleCaptureVideo()
{
  _sync = !_sync;
  _toggleVideo = !_toggleVideo;

#if USE_CAPTURE_VIDEO
  if (_toggleVideo == true)
  {
    for (auto &i : _windowManager) i->startCaptureVideo();
  }
  else
  {
    cout << "stopped video recording" << endl;
    for (auto &i : _windowManager) i->stopCaptureVideo();
  }
#endif // USE_CAPTURE_VIDEO
}

} // namespace yars
