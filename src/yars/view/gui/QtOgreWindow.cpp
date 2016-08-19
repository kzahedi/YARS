#include "QtOgreWindow.h"

#include "configuration/data/Data.h"
#include "configuration/YarsConfiguration.h"
#include "SceneGraphHandler.h"
#include "KeyHandler.h"
#include "SdkQtCameraMan.h"
#include "view/console/ConsoleView.h"

#define __NO_KEY  -1
#define __CONTROL  1
#define __ALT      2 

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
#include <Compositor/OgreCompositorManager2.h>
#endif

#define OGRE_TO_YARS(source, destination) \
  destination.x =  source[0]; \
destination.y = -source[2]; \
destination.z =  source[1];

#define YARS_TO_OGRE(source, destination) \
  destination[0] =  source.x; \
destination[1] =  source.z; \
destination[2] = -source.y;

#include <iostream>
#include <iomanip>

/*
   Note that we pass any supplied QWindow parent to the base QWindow class. This is necessary should we
   need to use our class within a container.
   */
QtOgreWindow::QtOgreWindow(int index, QWindow *parent)
  : QWindow(parent)
  , _update_pending(false)
  , _animating(false)
  , _ogreRoot(NULL)
  , _ogreWindow(NULL)
  , _ogreCamera(NULL)
  , _cameraMan(NULL)
{
  setAnimating(true);
  installEventFilter(this);

  _index                = index;
  _windowConfiguration  = new WindowConfiguration(index);
  _ogreBackground       = Ogre::ColourValue(0.8f, 0.8f, 0.8f);
  _captureRunning       = false;
  _captureStep          = 0;
  _capturedTenMinutes   = 0;
  _frameIndex           = 0;
  _capturingOffset      = 0;
  _imgCaptureFrameIndex = 0;
}

/*
   Upon destruction of the QWindow object we destroy the Ogre3D scene.
   */
QtOgreWindow::~QtOgreWindow()
{
  if (_cameraMan) delete _cameraMan;
  delete _ogreRoot;
#ifdef USE_CAPTURE_VIDEO
  if(_captureRunning) __closeMovie();
#endif // USE_CAPTURE_VIDEO
}

/*
   In case any drawing surface backing stores (QRasterWindow or QOpenGLWindow) of Qt are supplied to this
   class in any way we inform Qt that they will be unused.
   */
void QtOgreWindow::render(QPainter *painter)
{
  Q_UNUSED(painter);
}

/*
   Our initialization function. Called by our renderNow() function once when the window is first exposed.
   */
void QtOgreWindow::initialize()
{

#ifdef _MSC_VER
  _ogreRoot = new Ogre::Root(Ogre::String("plugins" OGRE_BUILD_SUFFIX ".cfg"));
#else
  Ogre::LogManager * lm = new Ogre::LogManager();
  lm->createLog("ogre.log", true, false, false); // create silent logging
  _ogreRoot = SceneGraphHandler::instance()->root();
#endif
  Ogre::ConfigFile ogreConfig;

  const Ogre::RenderSystemList& rsList = _ogreRoot->getAvailableRenderers();
  Ogre::RenderSystem* rs = rsList[0];

  Ogre::StringVector renderOrder;
#if defined(Q_OS_WIN)
  renderOrder.push_back("Direct3D9");
  renderOrder.push_back("Direct3D11");
#endif
  renderOrder.push_back("OpenGL");
  renderOrder.push_back("OpenGL 3+");
  for (Ogre::StringVector::iterator iter = renderOrder.begin(); iter != renderOrder.end(); iter++)
  {
    for (Ogre::RenderSystemList::const_iterator it = rsList.begin(); it != rsList.end(); it++)
    {
      if ((*it)->getName().find(*iter) != Ogre::String::npos)
      {
        rs = *it;
        break;
      }
    }
    if (rs != NULL) break;
  }
  if (rs == NULL)
  {
    if (!_ogreRoot->restoreConfig())
    {
      if (!_ogreRoot->showConfigDialog())
        OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
                    "Abort render system configuration",
                    "QtOgreWindow::initialize");
    }
  }

  int w = _windowConfiguration->geometry.width();
  int h = _windowConfiguration->geometry.height();
  QString dimensions = QString("%1 x %2").arg(w).arg(h);
  // rs->setConfigOption("Video Mode", dimensions.toStdString());
  rs->setConfigOption("Full Screen", "No");
  rs->setConfigOption("VSync", "Yes");
  _ogreRoot->setRenderSystem(rs);
  _ogreRoot->initialise(false);

  Ogre::NameValuePairList parameters;

  if (rs->getName().find("GL") <= rs->getName().size())
    parameters["currentGLContext"] = Ogre::String("false");

#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
  parameters["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)(this->winId()));
  parameters["parentWindowHandle"] = Ogre::StringConverter::toString((size_t)(this->winId()));
#else
  parameters["externalWindowHandle"] = Ogre::StringConverter::toString((unsigned long)(this->winId()));
  parameters["parentWindowHandle"] = Ogre::StringConverter::toString((unsigned long)(this->winId()));
#endif

#if defined(Q_OS_MAC)
  parameters["macAPI"] = "cocoa";
  parameters["macAPICocoaUseNSView"] = "true";
#endif

  stringstream sst;
  sst << "QT Window " << _index;
  resize(w,h);
  _ogreWindow = _ogreRoot->createRenderWindow(sst.str().c_str(),
                                              this->width(),
                                              this->height(),
                                              false,
                                              &parameters);
  _ogreWindow->setVisible(true);

  _ogreSceneMgr = SceneGraphHandler::instance()->sceneManager();

  sst.str("");
  sst << "Camera " << _index;
  _ogreCamera = _ogreSceneMgr->createCamera(sst.str().c_str());
  Ogre::Vector3 pos;
  Ogre::Vector3 lookAt;
  YARS_TO_OGRE(_windowConfiguration->cameraPosition, pos);
  YARS_TO_OGRE(_windowConfiguration->cameraLookAt,   lookAt);
  _ogreCamera->setPosition(pos);
  _ogreCamera->lookAt(lookAt);
  _ogreCamera->setNearClipDistance(0.01f);
  _ogreCamera->setFarClipDistance(1000000.0f);

  _cameraMan = new SdkQtCameraMan(_ogreCamera);   // create a default camera controller

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
  createCompositor();
#else
  _viewport = _ogreWindow->addViewport(_ogreCamera);
  _viewport->setBackgroundColour(_ogreBackground);
#endif

  _ogreCamera->setAspectRatio(Ogre::Real(_ogreWindow->getWidth()) / Ogre::Real(_ogreWindow->getHeight()));
  _ogreCamera->setAutoAspectRatio(true);

  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

  SceneGraphHandler::instance()->initialise();

  _ogreRoot->addFrameListener(this);
  setTitle(QString::fromStdString(_windowConfiguration->name));
}


#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
void QtOgreWindow::createCompositor()
{
  /*
     Example compositor
     Derive this class for your own purpose and overwite this function to have a working Ogre
     widget with your own compositor.
     */
  Ogre::CompositorManager2* compMan = _ogreRoot->getCompositorManager2();
  const Ogre::String workspaceName = "default scene workspace";
  const Ogre::IdString workspaceNameHash = workspaceName;
  compMan->createBasicWorkspaceDef(workspaceName, _ogreBackground);
  compMan->addWorkspace(_ogreSceneMgr, _ogreWindow, _ogreCamera, workspaceNameHash, true);
}
#endif

void QtOgreWindow::render()
{
  /*
     How we tied in the render function for OGre3D with QWindow's render function. This is what gets call
     repeatedly. Note that we don't call this function directly; rather we use the renderNow() function
     to call this method as we don't want to render the Ogre3D scene unless everything is set up first.
     That is what renderNow() does.

     Theoretically you can have one function that does this check but from my experience it seems better
     to keep things separate and keep the render function as simple as possible.
     */
  Ogre::WindowEventUtilities::messagePump();
  // _sceneGraph->update();
  // _ogreRoot->renderOneFrame();
}

void QtOgreWindow::renderLater()
{
  /*
     This function forces QWindow to keep rendering. Omitting this causes the renderNow() function to
     only get called when the window is resized, moved, etc. as opposed to all of the time; which is
     generally what we need.
     */
  if (!_update_pending)
  {
    _update_pending = true;
    QApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
  }
}

bool QtOgreWindow::event(QEvent *event)
{
  switch (event->type())
  {
    case QEvent::UpdateRequest:
      _update_pending = false;
      renderNow();
      return true;

    case QEvent::NativeGesture:
      _cameraMan->gesture((QNativeGestureEvent*)(event));
      return true;
      break;

    default:
      return QWindow::event(event);
  }
}



/*
   Called after the QWindow is reopened or when the QWindow is first opened.
   */
void QtOgreWindow::exposeEvent(QExposeEvent *event)
{
  Q_UNUSED(event);

  if (isExposed())
    renderNow();
}

/*
   The renderNow() function calls the initialize() function when needed and if the QWindow is already
   initialized and prepped calls the render() method.
   */
void QtOgreWindow::renderNow()
{
  if (!isExposed())
    return;

  if (_ogreRoot == NULL)
  {
    initialize();
  }

  render();

  if (_animating)
    renderLater();
}

/*
   Our event filter; handles the resizing of the QWindow. When the size of the QWindow changes note the
   call to the Ogre3D window and camera. This keeps the Ogre3D scene looking correct.
   */
bool QtOgreWindow::eventFilter(QObject *target, QEvent *event)
{
  if (target == this)
  {
    if (event->type() == QEvent::Resize)
    {
      if (isExposed() && _ogreWindow != NULL)
      {
        _ogreWindow->resize(this->width(), this->height());
      }
    }
  }

  return false;
}

/*
   How we handle keyboard and mouse events.
   */

void QtOgreWindow::mouseMoveEvent( QMouseEvent* e )
{
  static int lastX = e->x();
  static int lastY = e->y();
  int relX = e->x() - lastX;
  int relY = e->y() - lastY;
  lastX = e->x();
  lastY = e->y();

  if(_cameraMan && (e->buttons() & Qt::LeftButton))
    _cameraMan->injectMouseMove(relX, relY);
}

void QtOgreWindow::wheelEvent(QWheelEvent *e)
{
  if(_cameraMan)
    _cameraMan->injectWheelMove(*e);
}

void QtOgreWindow::mousePressEvent( QMouseEvent* e )
{
  if(_cameraMan)
    _cameraMan->injectMouseDown(*e);
}

void QtOgreWindow::mouseReleaseEvent( QMouseEvent* e )
{
  if(_cameraMan)
    _cameraMan->injectMouseUp(*e);

  // QPoint pos = e->pos();
  // Ogre::Ray mouseRay = _ogreCamera->getCameraToViewportRay(
                            // (Ogre::Real)pos.x() / _ogreWindow->getWidth(),
                            // (Ogre::Real)pos.y() / _ogreWindow->getHeight());
  // Ogre::RaySceneQuery* pSceneQuery = _ogreSceneMgr->createRayQuery(mouseRay);
  // pSceneQuery->setSortByDistance(true);
  // Ogre::RaySceneQueryResult vResult = pSceneQuery->execute();
  // for (size_t ui = 0; ui < vResult.size(); ui++)
  // {
    // if (vResult[ui].movable)
    // {
      // if (vResult[ui].movable->getMovableType().compare("Entity") == 0)
      // {
        // emit entitySelected((Ogre::Entity*)vResult[ui].movable);
      // }
    // }
  // }
  // _ogreSceneMgr->destroyQuery(pSceneQuery);
}

/*
   Function to keep track of when we should and shouldn't redraw the window; we wouldn't want to do
   rendering when the QWindow is minimized. This takes care of those scenarios.
   */
void QtOgreWindow::setAnimating(bool animating)
{
  _animating = animating;

  if (animating)
    renderLater();
}

bool QtOgreWindow::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
  _cameraMan->frameRenderingQueued(evt);
  return true;
}

void QtOgreWindow::log(Ogre::String msg)
{
  // if(Ogre::LogManager::getSingletonPtr() != NULL) Ogre::LogManager::getSingletonPtr()->logMessage(msg);
}

void QtOgreWindow::log(QString msg)
{
  // log(Ogre::String(msg.toStdString().c_str()));
}

void QtOgreWindow::keyPressEvent(QKeyEvent *event)
{
  if(_cameraMan) _cameraMan->injectKeyDown(*event);


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

  int i = KeyHandler::instance()->handleKeyEvent(alt, ctrl, shift, key);
  // __catchedLocally(i);
}

// void QtOgreWindow::__catchedLocally(int key)
// {
  // switch(key)
  // {
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
  // }
// }


void QtOgreWindow::keyReleaseEvent(QKeyEvent *event)
{
  if(_cameraMan) _cameraMan->injectKeyUp(*event);
  _metaKey = __NO_KEY;
}


#ifdef USE_CAPTURE_VIDEO
void QtOgreWindow::__initMovie()
{
  _windowConfiguration->getNextCaptureName();
  std::stringstream oss;
  __YARS_OPEN_CAPTURE_DIRECTORY;
  _captureStep = __YARS_GET_SIMULATOR_FREQUENCY / __YARS_GET_CAPTURE_FRAME_RATE;
  _capturedTenMinutes = 600 * __YARS_GET_CAPTURE_FRAME_RATE;
  uint width  = _viewport->getActualWidth();
  uint height = _viewport->getActualHeight();
  oss << __YARS_GET_CAPTURE_DIRECTORY << "/" << _windowConfiguration->captureName;
  _mov = quicktime_open(oss.str().c_str(),0,1);
  lqt_codec_info_t **codec = lqt_find_video_codec((char*)__YARS_GET_VIDEO_CODEC.c_str(),1);
  if(codec == NULL)
  {
    YarsErrorHandler::push("Video codec %s not found.", __YARS_GET_VIDEO_CODEC.c_str());
    exit(0);
  }
  cout << "Starting video capture of " << oss.str().c_str()
    << " with width " << width << ", height " << height
    << ", frame rate " << __YARS_GET_CAPTURE_FRAME_RATE
    << ", and " << __YARS_GET_VIDEO_CODEC.c_str()
    << " codec." << endl; 
  lqt_add_video_track(_mov, width, height, 1, __YARS_GET_CAPTURE_FRAME_RATE, codec[0]);
  quicktime_set_cmodel(_mov, BC_RGB888);
  _captureRunning = true;
  _capturingOffset = __YARS_GET_STEP;
  _frameIndex = 0;
  __initRenderFrame();
}

void QtOgreWindow::__closeMovie()
{
  Ogre::TextureManager::getSingleton().remove("StreamTex");
  quicktime_close(_mov);
  _captureRunning = false;
}

void QtOgreWindow::__captureMovieFrame()
{
  if(__YARS_GET_STEP % _captureStep != 0) return;
  if(__YARS_GET_USE_PAUSE) return;
  if(_frameIndex > 0 && _frameIndex % _capturedTenMinutes == 0)
  {
    __closeMovie();
    __initMovie();
  }
  uint width  = _viewport->getActualWidth();
  uint height = _viewport->getActualHeight();

  ConsoleView::printCapturingInformation(_frameIndex);

  uint8_t **a;
  int rowspan = 0;
  int row_uv  = 0;

  a = lqt_rows_alloc(width, height, BC_RGB888, &rowspan, &row_uv);
  lqt_set_row_span(_mov, 0, rowspan);
  lqt_set_row_span_uv(_mov, 0, row_uv);

  _pRenderTex = _renderTexture->getBuffer()->getRenderTarget();
  _pRenderTex->update();
  Ogre::HardwarePixelBufferSharedPtr buffer = _renderTexture->getBuffer();
  uint8_t *pData = (uint8_t*)buffer->lock(0, width*height*4, Ogre::HardwareBuffer::HBL_READ_ONLY);

  for(uint x = 0; x < width; x++)
  {
    for(uint y = 0; y < height; y++)
    {
      a[y][3 * x]     = pData[4 * (y * width + x) + 2];
      a[y][3 * x + 1] = pData[4 * (y * width + x) + 1];
      a[y][3 * x + 2] = pData[4 * (y * width + x) + 0];
    }
  }
  buffer->unlock();

  lqt_encode_video(_mov, a, 0, _frameIndex);
  lqt_rows_free(a);
  _frameIndex++;
}

void QtOgreWindow::captureVideo()
{
  if(_captureRunning) __captureMovieFrame();
}

bool QtOgreWindow::captureRunning()
{
  return _captureRunning;
}

void QtOgreWindow::__initRenderFrame()
{
  stringstream oss;
  oss << "render texture " << _index;
  if(!Ogre::TextureManager::getSingleton().resourceExists(oss.str()))
  {
    _renderTexture =
      Ogre::TextureManager::getSingleton().createManual(oss.str(),
                                                        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                        Ogre::TEX_TYPE_2D,
                                                        _viewport->getActualWidth(),
                                                        _viewport->getActualHeight(),
                                                        0,
                                                        Ogre::PF_B8G8R8A8,
                                                        Ogre::TU_RENDERTARGET);

    _pRenderTex = _renderTexture->getBuffer()->getRenderTarget();
    _pRenderTex->addViewport(_ogreCamera);

    Ogre::Viewport *vp = _pRenderTex->getViewport(0);
    vp->setClearEveryFrame(true);
    vp->setBackgroundColour(Ogre::ColourValue::Black);
    vp->setOverlaysEnabled(true);
  }
  __YARS_OPEN_FRAMES_DIRECTORY;
}

void QtOgreWindow::startCaptureVideo()
{
  __initMovie();
}

void QtOgreWindow::stopCaptureVideo()
{
  __closeMovie();
}

#endif // USE_CAPTURE_VIDEO

void QtOgreWindow::captureImageFrame()
{
  __initRenderFrame();
  ConsoleView::printCapturingInformation(_imgCaptureFrameIndex);
  _imgCaptureFrameIndex++;
  stringstream oss;
  oss << __YARS_GET_FRAMES_DIRECTORY << "/frame_" << setfill('0') << setw(8)
    << _imgCaptureFrameIndex       << ".png";
  _pRenderTex = _renderTexture->getBuffer()->getRenderTarget();
  _pRenderTex->update();
  _pRenderTex->writeContentsToFile(oss.str());
}
