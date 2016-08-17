#include "QtOgreWindow.h"

#include "SceneGraphHandler.h"

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

/*
   Note that we pass any supplied QWindow parent to the base QWindow class. This is necessary should we
   need to use our class within a container.
   */
QtOgreWindow::QtOgreWindow(int index, QWindow *parent)
  : QWindow(parent)
  , m_update_pending(false)
  , m_animating(false)
  , m_ogreRoot(NULL)
  , m_ogreWindow(NULL)
  , m_ogreCamera(NULL)
  // , m_cameraMan(NULL)
{
  _index                = index;
  _windowConfiguration  = new WindowConfiguration(index);

  setAnimating(true);
  installEventFilter(this);
  m_ogreBackground = Ogre::ColourValue(0.8f, 0.8f, 0.8f);
}

/*
   Upon destruction of the QWindow object we destroy the Ogre3D scene.
   */
QtOgreWindow::~QtOgreWindow()
{
  // if (m_cameraMan) delete m_cameraMan;
  delete m_ogreRoot;
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
  m_ogreRoot = new Ogre::Root(Ogre::String("plugins" OGRE_BUILD_SUFFIX ".cfg"));
#else
  Ogre::LogManager * lm = new Ogre::LogManager();
  lm->createLog("ogre.log", true, false, false); // create silent logging
  m_ogreRoot = SceneGraphHandler::instance()->root();
#endif
  Ogre::ConfigFile ogreConfig;

  const Ogre::RenderSystemList& rsList = m_ogreRoot->getAvailableRenderers();
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
    if (!m_ogreRoot->restoreConfig())
    {
      if (!m_ogreRoot->showConfigDialog())
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
  m_ogreRoot->setRenderSystem(rs);
  m_ogreRoot->initialise(false);

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

  /*
     Note below that we supply the creation function for the Ogre3D window the width and height
     from the current QWindow object using the "this" pointer.
     */
  m_ogreWindow = m_ogreRoot->createRenderWindow("QT Window",
                                                this->width(),
                                                this->height(),
                                                false,
                                                &parameters);
  m_ogreWindow->setVisible(true);

  /*
     The rest of the code in the initialization function is standard Ogre3D scene code. Consult other
     tutorials for specifics.
     */
// #if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
  // const size_t numThreads = std::max<int>(1, Ogre::PlatformInformation::getNumLogicalCores());
  // Ogre::InstancingTheadedCullingMethod threadedCullingMethod = Ogre::INSTANCING_CULLING_SINGLETHREAD;
  // if (numThreads > 1)threadedCullingMethod = Ogre::INSTANCING_CULLING_THREADED;
  // m_ogreSceneMgr = m_ogreRoot->createSceneManager(Ogre::ST_GENERIC, numThreads, threadedCullingMethod);
// #else
  // m_ogreSceneMgr = m_ogreRoot->createSceneManager(Ogre::ST_GENERIC);
// #endif

  m_ogreSceneMgr = SceneGraphHandler::instance()->sceneManager();

  m_ogreCamera = m_ogreSceneMgr->createCamera("MainCamera");
  // m_ogreCamera->setPosition(Ogre::Vector3(0.0f, 0.0f, 10.0f));
  // m_ogreCamera->lookAt(Ogre::Vector3(0.0f, 0.0f, -300.0f));
  cout << "Position: " << _windowConfiguration->cameraPosition << endl;
  cout << "Look At: "  << _windowConfiguration->cameraLookAt   << endl;
  Ogre::Vector3 pos;
  Ogre::Vector3 lookAt;
  YARS_TO_OGRE(_windowConfiguration->cameraPosition, pos);
  YARS_TO_OGRE(_windowConfiguration->cameraLookAt,   lookAt);
  m_ogreCamera->setPosition(pos);
  m_ogreCamera->lookAt(lookAt);
  m_ogreCamera->setNearClipDistance(0.01f);
  m_ogreCamera->setFarClipDistance(1000000.0f);

  // m_cameraMan = new OgreQtBites::SdkQtCameraMan(m_ogreCamera);   // create a default camera controller

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
  createCompositor();
#else
  Ogre::Viewport* pViewPort = m_ogreWindow->addViewport(m_ogreCamera);
  pViewPort->setBackgroundColour(m_ogreBackground);
#endif


  m_ogreCamera->setAspectRatio(Ogre::Real(m_ogreWindow->getWidth()) / Ogre::Real(m_ogreWindow->getHeight()));
  m_ogreCamera->setAutoAspectRatio(true);

  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

  createScene();

  m_ogreRoot->addFrameListener(this);
  resize(w,h);
  setTitle(QString::fromStdString(_windowConfiguration->name));
}

void QtOgreWindow::createScene()
{
  m_ogreSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));

  Ogre::SceneNode* childSceneNode = m_ogreSceneMgr->getRootSceneNode()->createChildSceneNode();

  _sceneGraph = new SceneGraph(childSceneNode, m_ogreSceneMgr);

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
  Ogre::SceneNode* pLightNode = m_ogreSceneMgr->getRootSceneNode()->createChildSceneNode();
  Ogre::Light* light = m_ogreSceneMgr->createLight();
  pLightNode->attachObject(light);
  pLightNode->setPosition(20.0f, 80.0f, 50.0f);
#else
  Ogre::Light* light = m_ogreSceneMgr->createLight("MainLight");
  light->setPosition(20.0f, 80.0f, 50.0f);
#endif
}

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
void QtOgreWindow::createCompositor()
{
  /*
     Example compositor
     Derive this class for your own purpose and overwite this function to have a working Ogre
     widget with your own compositor.
     */
  Ogre::CompositorManager2* compMan = m_ogreRoot->getCompositorManager2();
  const Ogre::String workspaceName = "default scene workspace";
  const Ogre::IdString workspaceNameHash = workspaceName;
  compMan->createBasicWorkspaceDef(workspaceName, m_ogreBackground);
  compMan->addWorkspace(m_ogreSceneMgr, m_ogreWindow, m_ogreCamera, workspaceNameHash, true);
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
  _sceneGraph->update();
  m_ogreRoot->renderOneFrame();
}

void QtOgreWindow::renderLater()
{
  /*
     This function forces QWindow to keep rendering. Omitting this causes the renderNow() function to
     only get called when the window is resized, moved, etc. as opposed to all of the time; which is
     generally what we need.
     */
  if (!m_update_pending)
  {
    m_update_pending = true;
    QApplication::postEvent(this, new QEvent(QEvent::UpdateRequest));
  }
}

bool QtOgreWindow::event(QEvent *event)
{
  /*
     QWindow's "message pump". The base method that handles all QWindow events. As you will see there
     are other methods that actually process the keyboard/other events of Qt and the underlying OS.

     Note that we call the renderNow() function which checks to see if everything is initialized, etc.
     before calling the render() function.
     */

  switch (event->type())
  {
    case QEvent::UpdateRequest:
      m_update_pending = false;
      renderNow();
      return true;

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

  if (m_ogreRoot == NULL)
  {
    initialize();
  }

  render();

  if (m_animating)
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
      if (isExposed() && m_ogreWindow != NULL)
      {
        m_ogreWindow->resize(this->width(), this->height());
      }
    }
  }

  return false;
}

/*
   How we handle keyboard and mouse events.
   */
void QtOgreWindow::keyPressEvent(QKeyEvent * ev)
{
  // if(m_cameraMan)
    // m_cameraMan->injectKeyDown(*ev);
}

void QtOgreWindow::keyReleaseEvent(QKeyEvent * ev)
{
  // if(m_cameraMan)
    // m_cameraMan->injectKeyUp(*ev);
}

void QtOgreWindow::mouseMoveEvent( QMouseEvent* e )
{
  static int lastX = e->x();
  static int lastY = e->y();
  int relX = e->x() - lastX;
  int relY = e->y() - lastY;
  lastX = e->x();
  lastY = e->y();

  // if(m_cameraMan && (e->buttons() & Qt::LeftButton))
    // m_cameraMan->injectMouseMove(relX, relY);
}

void QtOgreWindow::wheelEvent(QWheelEvent *e)
{
  // if(m_cameraMan)
    // m_cameraMan->injectWheelMove(*e);
}

void QtOgreWindow::mousePressEvent( QMouseEvent* e )
{
  // if(m_cameraMan)
    // m_cameraMan->injectMouseDown(*e);
}

void QtOgreWindow::mouseReleaseEvent( QMouseEvent* e )
{
  // if(m_cameraMan)
    // m_cameraMan->injectMouseUp(*e);

  QPoint pos = e->pos();
  Ogre::Ray mouseRay = m_ogreCamera->getCameraToViewportRay(
                                                            (Ogre::Real)pos.x() / m_ogreWindow->getWidth(),
                                                            (Ogre::Real)pos.y() / m_ogreWindow->getHeight());
  Ogre::RaySceneQuery* pSceneQuery = m_ogreSceneMgr->createRayQuery(mouseRay);
  pSceneQuery->setSortByDistance(true);
  Ogre::RaySceneQueryResult vResult = pSceneQuery->execute();
  for (size_t ui = 0; ui < vResult.size(); ui++)
  {
    if (vResult[ui].movable)
    {
      if (vResult[ui].movable->getMovableType().compare("Entity") == 0)
      {
        emit entitySelected((Ogre::Entity*)vResult[ui].movable);
      }
    }
  }
  m_ogreSceneMgr->destroyQuery(pSceneQuery);
}

/*
   Function to keep track of when we should and shouldn't redraw the window; we wouldn't want to do
   rendering when the QWindow is minimized. This takes care of those scenarios.
   */
void QtOgreWindow::setAnimating(bool animating)
{
  m_animating = animating;

  if (animating)
    renderLater();
}

bool QtOgreWindow::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
  // m_cameraMan->frameRenderingQueued(evt);
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
