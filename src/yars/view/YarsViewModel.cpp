#include "YarsViewModel.h"

#include <yars/configuration/data/Data.h>
#include <yars/configuration/YarsConfiguration.h>
#include <yars/defines/mutex.h>

#include "view/gui/SceneGraphHandler.h"

#include <Ogre/Ogre.h>

#include <iostream>

using namespace std;

YarsViewModel::YarsViewModel()
{
  _drawFequency    = 1;
  _visualiseCalled = 0;
  _run             = true;
  _sync            = false;
  _syncedStep      = false;
  _toggleVideo     = false;
  _toggleFrames    = false;
  YM_INIT;

  if(__YARS_GET_USE_VISUALISATION) initialiseView();
  if(__YARS_GET_USE_CAPTURE_CL)    toggleCaptureVideo();
}

YarsViewModel::~YarsViewModel()
{
  Y_DEBUG("YarsViewModel destructor called.");
  YM_CLOSE;
}

void YarsViewModel::initialiseView()
{
  DataRobotSimulationDescription *data = __YARS_CURRENT_DATA;
  if(!__YARS_GET_USE_VISUALISATION) return;
  if(data->screens() == NULL) return;
  FOREACHP(DataScreen*, i, data->screens()) if((*i)->autoShow()) createNewWindow();
}

void YarsViewModel::visualiseScene()
{
  YM_LOCK;
  if(!__YARS_GET_USE_VISUALISATION)
  {
    YM_UNLOCK;
    return;
  }
  if(__YARS_CURRENT_DATA->screens() == NULL)
  {
    YM_UNLOCK;
    return;
  }

  SceneGraphHandler::instance()->step();

  if(_toggleVideo  == true) FOREACH(QtOgreWindow*, i, _windowManager) (*i)->captureVideo();
  if(_toggleFrames == true) FOREACH(QtOgreWindow*, i, _windowManager) (*i)->captureImageFrame();

  YM_UNLOCK;
}

void YarsViewModel::reset()
{
  // _ogreHandler->reset();
  // FOREACH(SdlWindow*, i, _windowManager) (*i)->reset();
}

void YarsViewModel::quit()
{
  Y_DEBUG("YarsViewModel::quit called")
  _run = false;
  FOREACH(QtOgreWindow*, i, _windowManager) (*i)->quit();
  _windowManager.clear();
  Y_DEBUG("YarsViewModel::quit completed")
}

void YarsViewModel::createNewWindow()
{
  YM_LOCK;
  // SdlWindow *wm = new SdlWindow(_windowManager.size());
  // QtWindowHandler *wm = new QtWindowHandler(_windowManager.size());
  // cout << QThread::currentThreadId() << endl;
  QtOgreWindow *wm = new QtOgreWindow(_windowManager.size());
  wm->show();
  // wm->addObserver(this);
#ifdef USE_CAPTURE_VIDEO
  // if(wm->captureRunning())
  // {
    // _sync       = true;
    // _syncedStep = true;
  // }
#endif // USE_CAPTURE_VIDEO
  _windowManager.push_back(wm);
  YM_UNLOCK;
}

void YarsViewModel::notify(ObservableMessage *m)
{
  switch(m->type())
  {
    case __M_NEW_WINDOW:        createNewWindow();       break; // new    window
    case -2:                    __removeClosedWindows(); break; // closed
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

// void YarsViewModel::run()
// {
  // while(_run)
  // {
    // YM_LOCK;
    // if(_sync)
    // {
      // if(_syncedStep) 
      // {
        // visualiseScene();
// #ifdef USE_CAPTURE_VIDEO
        // if(_toggleVideo == true)
        // {
          // cout << " staring video" << endl;
          // _toggleVideo = false;
          // FOREACH(QtOgreWindow*, i, _windowManager) (*i)->startCaptureVideo();
        // }
          // cout << " capturing video" << endl;
        // FOREACH(QtOgreWindow*, i, _windowManager) (*i)->captureVideo();
// #endif // USE_CAPTURE_VIDEO
        // _syncedStep = false;
      // }
    // }
    // else
    // {
// #ifdef USE_CAPTURE_VIDEO
      // if(_toggleVideo == true)
      // {
        // _toggleVideo = false;
        // FOREACH(QtOgreWindow*, i, _windowManager) (*i)->stopCaptureVideo();
      // }
// #endif // USE_CAPTURE_VIDEO
      // visualiseScene();
    // }
    // YM_UNLOCK;
  // }
// }

void YarsViewModel::synched()
{
  if(!_sync) return;
  _syncedStep = true;
  while(_syncedStep) usleep(1000);
}

void YarsViewModel::toggleShadows()
{
  // FOREACH(SdlWindow*, i, _windowManager) (*i)->toggleShadows();
}

void YarsViewModel::toggleCaptureVideo()
{
  _sync        = !_sync;
  _toggleVideo = !_toggleVideo;
  __YARS_SET_SYNC_GUI(_sync);
  if(_toggleVideo == true) FOREACH(QtOgreWindow*, i, _windowManager) (*i)->startCaptureVideo();
  else                     FOREACH(QtOgreWindow*, i, _windowManager) (*i)->stopCaptureVideo();
}

void YarsViewModel::toggleCaptureFrames()
{
  _sync         = !_sync;
  _toggleFrames = !_toggleFrames;
  __YARS_SET_SYNC_GUI(_sync);
}
