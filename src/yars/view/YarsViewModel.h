#ifndef __YARS_VIEW_MODEL_H__
#define __YARS_VIEW_MODEL_H__

#include <yars/view/console/ConsoleView.h>
#include <yars/view/gui/OgreHandler.h>
#include <yars/view/gui/SdlWindow.h>

#include <SDL2/SDL.h>

namespace yars {

class YarsViewModel
{
  public:
    YarsViewModel();
    ~YarsViewModel();

    void initialiseView();
    void reset();
    void visualiseScene();

    void quit();
    void run();
    void synched();
    void toggleShadows();
    void toggleCaptureVideo();
    void createNewWindow();
    void cleanupWindows();

  private:
    void __initialiseFollowable();
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

} // namespace yars

#endif // __YARS_VIEW_MODEL_H__
