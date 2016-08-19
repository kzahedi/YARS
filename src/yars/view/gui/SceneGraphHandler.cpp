#include "SceneGraphHandler.h"
#include "configuration/data/Data.h"
#include "util/Directories.h"
#include "SceneGraph.h"
#include <yars/defines/mutex.h>

// #include <QThread>

// #include <Overlay/OgreOverlaySystem.h>

SceneGraphHandler* SceneGraphHandler::_me = NULL;

SceneGraphHandler* SceneGraphHandler::instance()
{
  if(_me == NULL) _me = new SceneGraphHandler();
  return _me;
}

SceneGraphHandler::SceneGraphHandler()
{

  // cout << "scene graph handler thread: " << QThread::currentThreadId() << endl;
  Ogre::LogManager * lm = new Ogre::LogManager();
  lm->createLog("ogre.log", true, false, false); // create silent logging
  _root = new Ogre::Root("plugins.cfg", "ogre.cfg", ""); // no log file created here (see 1 line above)

  // Load resource paths from config file
  if(Directories::doesFileExist("resources.cfg"))
  {
    Ogre::ConfigFile cf;
    cf.load("resources.cfg");

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
      secName = seci.peekNextKey();
      Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
      Ogre::ConfigFile::SettingsMultiMap::iterator i;
      for (i = settings->begin(); i != settings->end(); ++i)
      {
        typeName = i->first;
        archName = i->second;
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                                                                       archName, typeName, secName);
      }
    }
  }

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
  const size_t numThreads = std::max<int>(1, Ogre::PlatformInformation::getNumLogicalCores());
  Ogre::InstancingTheadedCullingMethod threadedCullingMethod = Ogre::INSTANCING_CULLING_SINGLETHREAD;
  if (numThreads > 1)threadedCullingMethod = Ogre::INSTANCING_CULLING_THREADED;
  _sceneManager = _root->createSceneManager(Ogre::ST_GENERIC, numThreads, threadedCullingMethod);
#else
  _sceneManager = _root->createSceneManager(Ogre::ST_GENERIC);
#endif

#if OGRE_VERSION >= ((2 << 16) | (0 << 8) | 0)
  Ogre::SceneNode* pLightNode = _sceneManager->getRootSceneNode()->createChildSceneNode();
  Ogre::Light* light = _sceneManager->createLight();
  pLightNode->attachObject(light);
  pLightNode->setPosition(20.0f, 80.0f, 50.0f);
#else
  Ogre::Light* light = _sceneManager->createLight("MainLight");
  light->setPosition(20.0f, 80.0f, 50.0f);
#endif

  _sceneGraphInitialised = false;
}

void SceneGraphHandler::reset()
{
  _sceneGraph->reset();
}

void SceneGraphHandler::step()
{
  YM_LOCK;
  _sceneGraph->update();
  _root->renderOneFrame();
  YM_UNLOCK;
}

Ogre::Root* SceneGraphHandler::root()
{
  return _root;
}

Ogre::SceneManager* SceneGraphHandler::sceneManager()
{
  return _sceneManager;
}

void SceneGraphHandler::initialise()
{
  if(_sceneGraphInitialised == true) return;
  _sceneManager->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
  _rootNode = _sceneManager->getRootSceneNode()->createChildSceneNode();
  _sceneGraph = new SceneGraph(_rootNode, _sceneManager);
  _sceneGraphInitialised = true;
}

