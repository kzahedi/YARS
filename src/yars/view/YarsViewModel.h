#ifndef __YARS_VIEW_MODEL_H__
#define __YARS_VIEW_MODEL_H__

#include "util/Observable.h"
#include "console/ConsoleView.h"
#include "view/gui/OgreHandler.h"
#include "gui/SdlWindow.h"

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
    void run();
    void synched();
    void toggleShadows();
    void toggleCaptureVideo();

  private:
    void __initialiseFollowable();
    void __removeClosedWindows();
    void __newWindow();
    void __createNewWindow();

    std::vector<SdlWindow*> _windowManager;
    int                     _drawFequency;
    int                     _visualiseCalled;
    bool                    _run;
    bool                    _sync;
    bool                    _syncedStep;
    bool                    _toggleVideo;
    OgreHandler            *_ogreHandler;
};

#endif // __YARS_VIEW_MODEL_H__
