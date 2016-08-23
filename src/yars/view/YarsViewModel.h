#ifndef __YARS_VIEW_MODEL_H__
#define __YARS_VIEW_MODEL_H__

#include "util/Observable.h"
#include "console/ConsoleView.h"
#include "view/gui/OgreHandler.h"
#include "gui/SdlWindow.h"
#include "SDL2/SDL.h"

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
    void createNewWindow();

  private:
    void __initialiseFollowable();
    void __removeClosedWindows();
    void __newWindow();
    void __createWindow();

    std::vector<SdlWindow*> _windowManager;
    std::vector<SdlWindow*> _newWindows;
    int                     _drawFequency;
    int                     _visualiseCalled;
    bool                    _run;
    bool                    _sync;
    bool                    _syncedStep;
    bool                    _toggleVideo;
    OgreHandler            *_ogreHandler;
    SDL_Event               _event;
    unsigned long           _timeStamp;
    int                     _first;
    int                     _last;
};

#endif // __YARS_VIEW_MODEL_H__
