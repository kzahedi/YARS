#ifndef __YARS_VIEW_MODEL_H__
#define __YARS_VIEW_MODEL_H__

#include "util/Observable.h"
#include "console/ConsoleView.h"
#include "view/gui/SceneGraphHandler.h"
// #include "gui/QtWindowHandler.h"
#include "gui/QtOgreWindow.h"
// #include "gui/SdlWindow.h"

class YarsViewModel : public Observable, public Observer
{
  public:
    YarsViewModel();
    ~YarsViewModel();

    void initialiseView();
    void reset();
    void visualiseScene();

    void quit();
    void notify(ObservableMessage *m);
    // void run();
    void synched();
    void toggleShadows();
    void toggleCaptureVideo();
    void toggleCaptureFrames();
    void createNewWindow();


  private:
    void __initialiseFollowable();
    void __removeClosedWindows();

    std::vector<QtOgreWindow*> _windowManager;

    int                _drawFequency;
    int                _visualiseCalled;
    bool               _run;
    bool               _sync;
    bool               _syncedStep;
    bool               _toggleVideo;
    bool               _toggleFrames;
    SceneGraphHandler* _ogreHandler;
    Ogre::Root*        _root;
    pthread_mutex_t    _mutex;
};

#endif // __YARS_VIEW_MODEL_H__
