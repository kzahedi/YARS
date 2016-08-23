#include "view/YarsViewModel.h"

#include "configuration/data/Data.h"
#include "configuration/YarsConfiguration.h"
#include "gui/GuiMutex.h"

#include <Ogre/Ogre.h>

YarsViewModel::YarsViewModel()
{
  _drawFequency    = 1;
  _visualiseCalled = 0;
  _run             = true;
  _sync            = false;
  _syncedStep      = false;
  _toggleVideo     = false;

  if(__YARS_GET_USE_VISUALISATION)
  {
    _ogreHandler                = OgreHandler::instance();
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

  while(SDL_PollEvent(&_event))
  {
    FOREACH(SdlWindow*, i, _windowManager) if((*i) != NULL) (*i)->handleEvent(_event);
  }
  
  FOREACH(SdlWindow*, i, _windowManager) if((*i) != NULL) (*i)->step();
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
  SdlWindow *wm = new SdlWindow(_windowManager.size());
  wm->addObserver(this);
#ifdef USE_CAPTURE_VIDEO
  if(wm->captureRunning())
  {
    _sync       = true;
    _syncedStep = true;
  }
#endif // USE_CAPTURE_VIDEO
  _newWindows.push_back(wm);
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
    // for(std::vector<SdlWindow*>::iterator nw = _newWindows.begin(); nw != _newWindows.end(); nw++)
    // {
      // if((*nw)->visible())
      // {
        // cout << (*nw)->visible() << endl;
        // _windowManager.push_back(*nw);
        // _newWindows.erase(nw);
      // }
    // }
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
