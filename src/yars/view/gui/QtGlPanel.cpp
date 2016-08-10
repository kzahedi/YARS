#include "QtGlPanel.h"

#include "KeyHandler.h"
#include "util/macros.h"
#include "util/OSD.h"
#include "util/Timer.h"

#include <iostream>

#include <QMouseEvent>

using namespace std;


#define __NO_KEY  -1
#define __CONTROL  1
#define __ALT      2 

QtGlPanel::QtGlPanel(WindowConfiguration *windowConfiguration, QWidget *parent)
  : QGLWidget(parent)
{
  setAutoBufferSwap(true);
  setFocusPolicy(Qt::StrongFocus); // to grab keyboard

  _metaKey                   = __NO_KEY;
  _windowConfigurationDialog = NULL;
  _lastTime                  = 0;
  _printTimeFPS              = 0.0;
  _printRealTimeFactor       = 0.0;
  _printTimeLastStep         = 0;

  connect(this, SIGNAL(toggleFollowMode()),   parent, SLOT(toggleFollowingOfObjects()));
  connect(this, SIGNAL(previousFollowMode()), parent, SLOT(previousFollowMode()));
  connect(this, SIGNAL(nextFollowMode()),     parent, SLOT(nextFollowMode()));
  connect(this, SIGNAL(previousFollowable()), parent, SLOT(previousFollowable()));
  connect(this, SIGNAL(nextFollowable()),     parent, SLOT(nextFollowable()));
  connect(this, SIGNAL(writeFrames()),        parent, SLOT(toggleWriteFrames()));
  connect(this, SIGNAL(openNewWindow()),      parent, SLOT(openNewWindow()));
  connect(this, SIGNAL(setWindowTitle()),     parent, SLOT(openWindowTitleDialog()));
  connect(this, SIGNAL(setWindowSize()),      parent, SLOT(setWindowSize()));
  connect(this, SIGNAL(signalWindowConfigurationChanged()),
      parent, SLOT(windowConfigurationChanged()));
#ifdef USE_CAPTURE_VIDEO
  connect(this, SIGNAL(captureVideo()),       parent, SLOT(toggleCaptureMovie()));
#endif // USE_CAPTURE_VIDEO

  _imageFrameCounter           = 0;
#ifdef USE_CAPTURE_VIDEO
  _captureFrameCounter         = 0;
  _captureMovie                = false;
#endif // USE_CAPTURE_VIDEO
  _writeFrames                 = false;

  _windowConfiguration = windowConfiguration;
  // _glWrapper = NULL;
}

void QtGlPanel::initializeGL()
{
  // if(_glWrapper != NULL)
  // {
    // delete _glWrapper;
  // }
  // _glWrapper = new GlWrapper(_windowConfiguration);
}

void QtGlPanel::resizeGL(int w, int h)
{
  _windowConfiguration->geometry.setWidth(w);
  _windowConfiguration->geometry.setHeight(h);
  // _glWrapper->resize();
}

void QtGlPanel::paintGL()
{
  // _glWrapper->drawScene();

  if( !__YARS_GET_USE_PAUSE && _writeFrames)
  {
    // _glWrapper->captureFrame( _imageFrameCounter );
    _imageFrameCounter++;
  }

#ifdef USE_CAPTURE_VIDEO
  if( !__YARS_GET_USE_PAUSE && _captureMovie)
  {
    // _glWrapper->captureMovieFrame( _captureFrameCounter );
    _captureFrameCounter++;
  }
#endif // USE_CAPTURE_VIDEO

  // osd is not captured by video or frames
  if(_windowConfiguration->onScreenDisplay)
  {
    __osd();
  }

}

void QtGlPanel::mousePressEvent(QMouseEvent *event)
{
  _lastPos = event->pos();
}

void QtGlPanel::mouseMoveEvent(QMouseEvent *event)
{
  int dx = event->x() - _lastPos.x();
  int dy = event->y() - _lastPos.y();
  _lastPos = event->pos();
  bool b = _windowConfiguration->followObjects;
  if(b) emit toggleFollowMode();
#ifdef __APPLE__
  __mouseMotion(dx, dy);
#else // __APPLE__
  __mouseMotion(dx, dy, event->buttons());
#endif // __APPLE__
  if(b) toggleFollowMode();
}


void QtGlPanel::keyPressEvent(QKeyEvent *event)
{
  int  modifiers = event->modifiers();
  char key       = (char)event->key();

  bool alt   = false;
  bool ctrl  = false;
  bool shift = false;

  _metaKey = __NO_KEY;

  if(modifiers & Qt::ControlModifier)
  {
    ctrl  = true;
    _metaKey = __CONTROL;
  }

  if(modifiers & Qt::AltModifier)
  {
    alt   = true;
    _metaKey = __ALT;
  }

  if(modifiers & Qt::ShiftModifier)
  {
    shift = true;
  }

  // int i = YarsContainers::KeyEventHandler()->handleKeyEvent(alt, ctrl, shift, key);
  int i = KeyHandler::instance()->handleKeyEvent(alt, ctrl, shift, key);
  __catchedLocally(i);
}

void QtGlPanel::__catchedLocally(int key)
{
  switch(key)
  {
    // case 22: //YarsKeyFunction::PrintViewPoint:
      // ConsoleView::printViewpoint(_windowConfiguration->cameraPose.position, _windowConfiguration->cameraPose.orientation);
      // break;
    // case YarsKeyFunction::CaptureVideo:
      // emit captureVideo();
      // break;
    // case YarsKeyFunction::WriteFrames:
      // emit writeFrames();
      // break;
    // case YarsKeyFunction::VisualiseAxes:
      // _windowConfiguration->visualiseAxes = !_windowConfiguration->visualiseAxes;
      // break;
    // case YarsKeyFunction::OpenNewWindow:
      // emit openNewWindow();
      // break;
    // case YarsKeyFunction::SetWindowTitle:
      // emit setWindowTitle();
      // break;
    // case YarsKeyFunction::ShowWindowConfigurationDialog:
      // __showDialog();
      // break;
    // case YarsKeyFunction::SetWindowSize:
      // emit setWindowSize();
      // break;
    // case YarsKeyFunction::OnScreenDisplay_FramesPerSecond:
      // _windowConfiguration->osdFramePerSecond = !_windowConfiguration->osdFramePerSecond;
      // break;
    // case YarsKeyFunction::OnScreenDisplay_ElapsedTime:
      // _windowConfiguration->osdElapsedTime = !_windowConfiguration->osdElapsedTime;
      // break;
    // case YarsKeyFunction::OnScreenDisplay:
      // _windowConfiguration->onScreenDisplay = !_windowConfiguration->onScreenDisplay;
      // break;
    // case YarsKeyFunction::ToggleTextures:
      // _windowConfiguration->useTextures = !_windowConfiguration->useTextures;
      // break;
    // case YarsKeyFunction::ToggleShadows:
      // _windowConfiguration->useShadows = !_windowConfiguration->useShadows;
      // break;
    // case YarsKeyFunction::ToggleFollowMode:
      // emit toggleFollowMode();
      // break;
    // case YarsKeyFunction::ToggleTraces:
      // _windowConfiguration->useTraces = !_windowConfiguration->useTraces;
      // break;
    // case YarsKeyFunction::PreviousFollowable:
      // emit previousFollowable();
      // break;
    // case YarsKeyFunction::NextFollowable:
      // emit nextFollowable();
      // break;
    // case YarsKeyFunction::PreviousFollowMode:
      // emit previousFollowMode();
      // break;
    // case YarsKeyFunction::NextFollowMode:
      // emit nextFollowMode();
      // break;
  }
}


void QtGlPanel::keyReleaseEvent(QKeyEvent *event)
{
  _metaKey = __NO_KEY;
}

#ifdef __APPLE__
void QtGlPanel::__mouseMotion(int deltax, int deltay)
{
  float side = 0.01f * float(deltax);
  float fwd = (true) ? (0.01f * float(deltay)) : 0.0f;
  float s = (float) sin (_windowConfiguration->cameraPose.orientation.z);
  float c = (float) cos (_windowConfiguration->cameraPose.orientation.z);

  switch(_metaKey)
  {
    case __NO_KEY:
      _windowConfiguration->cameraPose.orientation.z += float (deltax) * 0.005f;
      _windowConfiguration->cameraPose.orientation.y += float (deltay) * 0.005f;
      break;
    case __ALT:
      _windowConfiguration->cameraPose.position.z += 0.01f * float(deltay);
      break;
    case __CONTROL:
      _windowConfiguration->cameraPose.position.x += -s*-side + c*-fwd;
      _windowConfiguration->cameraPose.position.y +=  c*-side + s*-fwd;
      break;
  }
  __wrapCameraAngles();
}
#else
void QtGlPanel::__mouseMotion(int deltax, int deltay, Qt::MouseButtons button)
{
  float side = 0.01f * float(deltax);
  float fwd = (true) ? (0.01f * float(deltay)) : 0.0f;
  float s = (float) sin (DEG_TO_RAD(_windowConfiguration->cameraPose.orientation.z));
  float c = (float) cos (DEG_TO_RAD(_windowConfiguration->cameraPose.orientation.z));

  switch(button)
  {
    case Qt::LeftButton:
      _windowConfiguration->cameraPose.orientation.z += float (deltax) * 0.5f;
      _windowConfiguration->cameraPose.orientation.y += float (deltay) * 0.5f;
      break;
    case Qt::MidButton:
      _windowConfiguration->cameraPose.position.z += 0.01f * float(deltay);
      break;
    case Qt::RightButton:
      _windowConfiguration->cameraPose.position.x += -s*-side + c*-fwd;
      _windowConfiguration->cameraPose.position.y +=  c*-side + s*-fwd;
      break;
  }
 __wrapCameraAngles();
}
#endif

void QtGlPanel::__wrapCameraAngles()
{
  // while (_windowConfiguration->cameraPose.orientation.x >  180) _windowConfiguration->cameraPose.orientation.x -= 360;
  // while (_windowConfiguration->cameraPose.orientation.x < -180) _windowConfiguration->cameraPose.orientation.x += 360;
  // while (_windowConfiguration->cameraPose.orientation.y >  180) _windowConfiguration->cameraPose.orientation.y -= 360;
  // while (_windowConfiguration->cameraPose.orientation.y < -180) _windowConfiguration->cameraPose.orientation.y += 360;
  // while (_windowConfiguration->cameraPose.orientation.z >  180) _windowConfiguration->cameraPose.orientation.z -= 360;
  // while (_windowConfiguration->cameraPose.orientation.z < -180) _windowConfiguration->cameraPose.orientation.z += 360;
}

void QtGlPanel::__showDialog()
{
  if(_windowConfigurationDialog == NULL)
  {
    _windowConfigurationDialog = new WindowConfigurationDialog(_windowConfiguration, this);
  }
  _windowConfigurationDialog->show();
}

void QtGlPanel::windowConfigurationChanged()
{
  emit signalWindowConfigurationChanged();
}

void QtGlPanel::__osd()
{
  if( Timer::getTime() - _lastTime > 500)
  {
    if( _windowConfiguration->osdElapsedTime )
    {
      string elapsedTime = OSD::getElapsedTimeString();
      _elapsedTimeString = QString(elapsedTime.c_str());
    }

    if( _windowConfiguration->osdFramePerSecond )
    {
      _printTimeFPS = ( ((float) __YARS_GET_STEP - _printTimeLastStep ) / ( ((double)Timer::getTime() - _lastTime)/1000.0));
      _printTimeLastStep = __YARS_GET_STEP;
      _printRealTimeFactor = _printTimeFPS / ((double)__YARS_GET_SIMULATOR_FREQUENCY);
      char buf[1024];
      sprintf(buf,"%.2f fps", _printTimeFPS);
      _fpsString1 = QString(buf);
      sprintf(buf,"%.2f RT", _printRealTimeFactor );
      _fpsString2 = QString(buf);
    }
    _lastTime = Timer::getTime();
  }
  //qglColor(QColor(142,198,219));
  qglColor(QColor(128,0,64));

  int height = 0;
  if( _windowConfiguration->osdElapsedTime )
  {
    // QFontMetrics fm(_windowConfiguration->osdElapsedTimeFont);
    // int width = fm.width(_elapsedTimeString);
    // height += fm.height() + 5;
    // renderText(
        // _windowConfiguration->geometry.width() - width - 10,
        // height,
        // _elapsedTimeString,
        // _windowConfiguration->osdElapsedTimeFont);
  }

  if( _windowConfiguration->osdFramePerSecond )
  {
    // QFontMetrics fm(_windowConfiguration->osdFramePerSecondFont);
    // int width  = fm.width(_fpsString1);
    // height += fm.height() + 5;
    // renderText(
        // _windowConfiguration->geometry.width() - width - 10,
        // height ,
        // _fpsString1,
        // _windowConfiguration->osdElapsedTimeFont);

    // width  = fm.width(_fpsString2);
    // height += fm.height() + 5;
    // renderText(
        // _windowConfiguration->geometry.width() - width - 10,
        // height ,
        // _fpsString2, _windowConfiguration->osdElapsedTimeFont);
  }
}

void QtGlPanel::toggleWriteFrames()
{
  _writeFrames = !_writeFrames;
  if(_writeFrames)
  {
    _imageFrameCounter = 0;
  }
}

#ifdef USE_CAPTURE_VIDEO
void QtGlPanel::toggleCaptureMovie()
{
  _captureMovie = !_captureMovie;
  if(_captureMovie)
  {
    _captureFrameCounter = 0;
    // _glWrapper->initMovie();
  }
  else
  {
    // _glWrapper->closeMovie();
  }
}
#endif // USE_CAPTURE_VIDEO

void QtGlPanel::reset()
{
}

void QtGlPanel::quit()
{
#ifdef USE_CAPTURE_VIDEO
  // _glWrapper->closeMovie();
#endif // USE_CAPTURE_VIDEO
}
