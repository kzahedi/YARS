#ifndef __YARS_VIEW_MODEL_H__
#define __YARS_VIEW_MODEL_H__

#include <yars/view/console/ConsoleView.h>
#include <yars/view/gui/OgreHandler.h>
#include <yars/view/gui/SdlWindow.h>

#include <SDL2/SDL.h>
#include <memory>

namespace yars {

class YarsViewModel
{
  public:
    YarsViewModel();
    ~YarsViewModel() = default;

    void initialiseView();
    void reset();
    void visualiseScene();

    void quit();
    void run();
    void synched();
    void toggleShadows();
    void toggleCaptureVideo();
    void createNewWindow();

  private:
    void __initialiseFollowable();
    void __newWindow();
    void __createWindow();

    std::vector<std::unique_ptr<SdlWindow>> _windowManager;
    std::vector<std::unique_ptr<SdlWindow>> _newWindows;
    int                     _drawFequency;
    int                     _visualiseCalled;
    volatile bool           _run;
    volatile bool           _sync;
    volatile bool           _syncedStep;
    bool                    _toggleVideo;
    OgreHandler            *_ogreHandler;
    SDL_Event               _event;
    unsigned long           _timeStamp;
    int                     _first;
    int                     _last;
};

} // namespace yars

#endif // __YARS_VIEW_MODEL_H__
