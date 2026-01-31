#include "YarsViewModel.h"

#include <yars/configuration/data/Data.h>
#include <yars/configuration/YarsConfiguration.h>
#include <yars/view/gui/GuiMutex.h>
#include <yars/util/Timer.h>

#include <OGRE/Ogre.h>
#include <algorithm>

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

  if (__YARS_GET_USE_VISUALISATION)
  {
    _ogreHandler = OgreHandler::instance();
    initialiseView();
    _ogreHandler->setupSceneManager();
    for (auto& w : _windowManager)
      if (w) w->setupOSD();
    if (__YARS_GET_USE_CAPTURE_CL)
      toggleCaptureVideo();
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
  if (data->screens() == nullptr)
    return;
  for (auto i = data->screens()->begin(); i != data->screens()->end(); ++i)
    if ((*i)->autoShow())
      __createWindow();
}

void YarsViewModel::visualiseScene()
{
  if (!__YARS_GET_USE_VISUALISATION)
    return;
  if (__YARS_CURRENT_DATA->screens() == nullptr)
    return;
  _ogreHandler->step();

  // Swap buffers for all windows after OGRE renders
  for (auto& w : _windowManager)
    if (w) w->swapBuffers();

  for (auto& w : _windowManager)
    if (w) w->step();
  while (SDL_PollEvent(&_event))
  {
    for (auto& w : _windowManager)
      if (w) w->handleEvent(_event);
  }
}

void YarsViewModel::reset()
{
  _ogreHandler->reset();
  for (auto& w : _windowManager)
    w->reset();
}

void YarsViewModel::quit()
{
  Y_DEBUG("YarsViewModel::quit called")
  _run = false;
  for (auto& w : _windowManager)
    w->quit();
  _windowManager.clear();
  Y_DEBUG("YarsViewModel::quit completed")
}

void YarsViewModel::__createWindow()
{
  auto wm = std::make_unique<SdlWindow>(_windowManager.size());
  wm->addObserver(this);
  wm->wait();  // Wait for window to be visible before adding to manager
  _windowManager.push_back(std::move(wm));
}

void YarsViewModel::createNewWindow()
{
  auto wm = std::make_unique<SdlWindow>(_windowManager.size() + _newWindows.size());
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
  _newWindows.push_back(std::move(wm));
  _timeStamp = Timer::getTime();
  _first++;
}

void YarsViewModel::notify(ObservableMessage *m)
{
  switch (m->type())
  {
  // case __M_NEW_WINDOW:        __newWindow();           break; // new    window
  // case -2:                    __removeClosedWindows(); break; // closed
  case __M_QUIT_CALLED:
    _run = false;
    break;
  case __M_TOGGLE_SYNCED_GUI:
    _sync = !_sync;
    break;
  case __M_CLOSE_WINDOW:
    cleanupWindows();
    break;
  }
}

void YarsViewModel::cleanupWindows()
{
  // Close windows marked as closed
  for (auto& w : _windowManager)
  {
    if (w && w->closed())
      w->close();
  }
  // Remove closed windows using erase-remove idiom
  _windowManager.erase(
    std::remove_if(_windowManager.begin(), _windowManager.end(),
      [](const std::unique_ptr<SdlWindow>& w) { return !w || w->closed(); }),
    _windowManager.end());
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
          for (auto& w : _windowManager)
            w->captureVideo();
        }
#endif // USE_CAPTURE_VIDEO
        _syncedStep = false;
      }
    }
    else
    {
      visualiseScene();
    }

    for (auto& w : _newWindows)
    {
      if (w && !w->added())
      {
        w->setAdded();
        _windowManager.push_back(std::move(w));
      }
    }
    // Remove moved-from windows from _newWindows
    _newWindows.erase(
      std::remove_if(_newWindows.begin(), _newWindows.end(),
        [](const std::unique_ptr<SdlWindow>& w) { return !w; }),
      _newWindows.end());
  }
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
  for (auto& w : _windowManager)
    w->toggleShadows();
}

void YarsViewModel::toggleCaptureVideo()
{
  _sync = !_sync;
  _toggleVideo = !_toggleVideo;

#if USE_CAPTURE_VIDEO
  if (_toggleVideo == true)
  {
    for (auto& w : _windowManager)
      w->startCaptureVideo();
  }
  else
  {
    cout << "stopped video recording" << endl;
    for (auto& w : _windowManager)
      w->stopCaptureVideo();
  }
#endif // USE_CAPTURE_VIDEO
}
