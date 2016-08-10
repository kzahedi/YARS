#ifndef __QT_WINDOW_HANDLER_H__
#define __QT_WINDOW_HANDLER_H__

#include "QtGlPanel.h"
#include "CameraHandler.h"
#include <yars/util/Observable.h>
#include "WindowConfiguration.h"

#include <QApplication>

class QtWindowHandler : public QFrame, public Observable
{
  Q_OBJECT
  public:
    QtWindowHandler(int index = 0);
    ~QtWindowHandler();
    void step();
    void reset();
    void quit();

    void closeEvent(QCloseEvent *event);
    bool closed();

    bool captureRunning() {return false;};


  public slots:
    void menuTriggered(QAction *action);
    void toggleFollowingOfObjects();
    void previousFollowMode();
    void nextFollowMode();
    void nextFollowable();
    void previousFollowable();
    void toggleWriteFrames();
    void openNewWindow();
    void openWindowTitleDialog();
    void setWindowSize();
    void windowConfigurationChanged();

#ifdef USE_CAPTURE_VIDEO
    void toggleCaptureMovie();
#endif // USE_CAPTURE_VIDEO

  private:
    void __createMenu();
    void __initialiseFollowable();
    void __openScenarioWindow();

    QtGlPanel                 *_glPanel;
    QMenu                     *_openWindows;
    QAction                   *_scenarioWindowAction;
    std::vector<std::string>   _logNames;
    std::vector<std::string>   _freeNames;
    CameraHandler             *_cameraHandler;
    bool                       _firstTimeFollowablesCalled;
    bool                       _followModeWasActivated;
    bool                       _closed;
    // EntitiesVector::iterator   _followedObject;
    ObservableMessage         *_newWindow;
    ObservableMessage         *_windowClosed;
    WindowConfiguration       *_windowConfiguration;
    // ScenarioWindow            *_scenarioWindow;
};
#endif // __QT_WINDOW_HANDLER_H__

