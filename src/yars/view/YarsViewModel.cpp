#include "view/YarsViewModel.h"

#include "configuration/data/Data.h"
#include "configuration/YarsConfiguration.h"
#include "gui/GuiMutex.h"
#include "util/Timer.h"

#include <Ogre/Ogre.h>

YarsViewModel::YarsViewModel()
{
  _drawFequency    = 1;
  _visualiseCalled = 0;
  _run             = true;
  _sync            = false;
  _syncedStep      = false;
  _toggleVideo     = false;
  _timeStamp       = 0;
  _first           = -1;
  _last            = 0;

  if(__YARS_GET_USE_VISUALISATION)
  {
    _ogreHandler = OgreHandler::instance();
    initialiseView();
    _ogreHandler->setupSceneManager();
    FOREACH(SdlWindow*, i, _windowManager) if((*i) != NULL) (*i)->setupOSD();
  }
}

YarsViewModel::~YarsViewModel()
{
  Y_DEBUG("YarsViewModel destructor called.");
}

void YarsViewModel::initialiseView()
{
  DataRobotSimulationDescription *data = __YARS_CURRENT_DATA;
  if(!__YARS_GET_USE_VISUALISATION) return;
  if(data->screens() == NULL) return;
  FOREACHP(DataScreen*, i, data->screens()) if((*i)->autoShow()) __createWindow();
}

void YarsViewModel::visualiseScene()
{
  if(!__YARS_GET_USE_VISUALISATION) return;
  if(__YARS_CURRENT_DATA->screens() == NULL) return;
  _ogreHandler->step();

  FOREACH(SdlWindow*, i, _windowManager) if((*i) != NULL) (*i)->step();
  while(SDL_PollEvent(&_event))
  {
    FOREACH(SdlWindow*, i, _windowManager) if((*i) != NULL) (*i)->handleEvent(_event);
  }
  
}

void YarsViewModel::reset()
{
  _ogreHandler->reset();
  FOREACH(SdlWindow*, i, _windowManager) (*i)->reset();
}

void YarsViewModel::quit()
{
  Y_DEBUG("YarsViewModel::quit called")
  _run = false;
  FOREACH(SdlWindow*, i, _windowManager) (*i)->quit();
  _windowManager.clear();
  Y_DEBUG("YarsViewModel::quit completed")
}

void YarsViewModel::__createWindow()
{
  SdlWindow *wm = new SdlWindow(_windowManager.size());
  wm->addObserver(this);
#ifdef USE_CAPTURE_VIDEO
  if(wm->captureRunning())
  {
    _sync       = true;
    _syncedStep = true;
  }
#endif // USE_CAPTURE_VIDEO
  _windowManager.push_back(wm);
}

void YarsViewModel::createNewWindow()
{
  SdlWindow *wm = new SdlWindow(_windowManager.size() + _newWindows.size());
  wm->addObserver(this);
#ifdef USE_CAPTURE_VIDEO
  if(wm->captureRunning())
  {
    _sync       = true;
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

void YarsViewModel::notify(ObservableMessage *m)
{
  switch(m->type())
  {
    // case __M_NEW_WINDOW:        __newWindow();           break; // new    window
    // case -2:                    __removeClosedWindows(); break; // closed
    case __M_QUIT_CALLED:       _run = false;            break;
    case __M_TOGGLE_SYNCED_GUI: _sync = !_sync;          break;
  }
}

void YarsViewModel::__removeClosedWindows()
{
  // for(std::vector<QtWindowHandler*>::iterator i = _windowManagers.begin();
  //     i != _windowManagers.end();
  //     i++)
  // {
  //   if((*i)->closed())
  //   {
  //     _windowManagers.erase(i);
  //     i--;
  //   }
  // }
}

void YarsViewModel::run()
{
  while(_run)
  {
    if(_sync)
    {
      if(_syncedStep) 
      {
        visualiseScene();
#ifdef USE_CAPTURE_VIDEO
        if(_toggleVideo == true)
        {
          _toggleVideo = false;
          FOREACH(SdlWindow*, i, _windowManager) (*i)->startCaptureVideo();
        }
        FOREACH(SdlWindow*, i, _windowManager) (*i)->captureVideo();
#endif // USE_CAPTURE_VIDEO
        _syncedStep = false;
      }
    }
    else
    {
#ifdef USE_CAPTURE_VIDEO
      if(_toggleVideo == true)
      {
        _toggleVideo = false;
        FOREACH(SdlWindow*, i, _windowManager) (*i)->stopCaptureVideo();
      }
#endif // USE_CAPTURE_VIDEO
      visualiseScene();
    }

    for(std::vector<SdlWindow*>::iterator i = _newWindows.begin(); i != _newWindows.end(); i++)
    {
      if((*i)->added() == false) 
      {
        _windowManager.push_back(*i);
        (*i)->setAdded();
      }
    }
  }
}

void YarsViewModel::synched()
{
  if(!_sync) return;
  _syncedStep = true;
  while(_syncedStep) usleep(1000);
}

void YarsViewModel::toggleShadows()
{
  FOREACH(SdlWindow*, i, _windowManager) (*i)->toggleShadows();
}

void YarsViewModel::toggleCaptureVideo()
{
  _sync        = !_sync;
  _toggleVideo = !_toggleVideo;
}
