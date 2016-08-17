#ifndef __QT_PANEL_H__
#define __QT_PANEL_H__

#include <yars/types/P3D.h>
#include <yars/types/ScreenGeometry.h>

#include "WindowConfiguration.h"
#include "WindowConfigurationDialog.h"
#include "SceneGraphHandler.h"

#include <QGLWidget>

class QtGlPanel : public QGLWidget
{
  Q_OBJECT
  public:
    QtGlPanel(WindowConfiguration *windowConfiguration, QWidget *parent = 0);
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void reset();
    void postProcessing();
    // void follow(dGeomID);
    void quit();

    void toggleWriteFrames();
#ifdef USE_CAPTURE_VIDEO
    void toggleCaptureMovie();
#endif // USE_CAPTURE_VIDEO


  protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

  public slots:
    void windowConfigurationChanged();

  signals:
    void signalWindowConfigurationChanged();

    void toggleFollowMode();

    void previousFollowable();
    void nextFollowable();

    void previousFollowMode();
    void nextFollowMode();

    void captureVideo();
    void writeFrames();

    void openNewWindow();

    void setWindowTitle();
    void setWindowSize();

  private:
    void __osd();
#ifdef __APPLE__
    void __mouseMotion(int deltax, int deltay);
#else // __APPLE__
    void __mouseMotion(int deltax, int deltay, Qt::MouseButtons button);
#endif // __APPLE__
    void __wrapCameraAngles();
    void __catchedLocally(int key);
    bool __checkKey(int key);
    void __showDialog();

    QPoint  _lastPos;
    int     _metaKey;
    QString _elapsedTimeString;
    QString _fpsString1;
    QString _fpsString2;
    double _printTimeFPS;
    double _printRealTimeFactor;
    unsigned long _printTimeLastStep;

    unsigned long _lastTime;

    WindowConfiguration*       _windowConfiguration;
    WindowConfigurationDialog* _windowConfigurationDialog;
    SceneGraphHandler*         _ogreHandler;

    // GlWrapper *_glWrapper;

    bool _writeFrames;
    int _imageFrameCounter;
#ifdef USE_CAPTURE_VIDEO
    bool _captureMovie;
    int _captureFrameCounter;
#endif // USE_CAPTURE_VIDEO

};
#endif // __QT_PANEL_H__
