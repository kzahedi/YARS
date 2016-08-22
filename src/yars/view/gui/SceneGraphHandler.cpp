#include "SceneGraphHandler.h"
#include "configuration/data/Data.h"
#include "util/Directories.h"
#include "SceneGraph.h"

#include <OgreOverlay/OgreOverlayManager.h>
#include <OgreFontManager.h>
#include <iostream>

using namespace std;

#include <QThread>

// #include <Overlay/OgreOverlaySystem.h>

SceneGraphHandler* SceneGraphHandler::_me = NULL;

SceneGraphHandler* SceneGraphHandler::instance()
{
  // cout << "SceneGraphHandler: " << getpid() << " " <<  pthread_self() << " " << QThread::currentThreadId() << endl;
  // mLOCK;
  if(_me == NULL) _me = new SceneGraphHandler();
  // mUNLOCK;
  return _me;
}

SceneGraphHandler::SceneGraphHandler()
{
  mLOCK;
  cout << "SceneGraphHandler Constructor" << endl;
  // cout << "scene graph handler thread: " << QThread::currentThreadId() << endl;
  Ogre::LogManager * lm = new Ogre::LogManager();
  lm->createLog("ogre.log", true, false, false); // create silent logging
  // no log file created here (see 1 line above)
  _root           = new Ogre::Root("plugins.cfg", "ogre.cfg", "");
  _overlayManager = new Ogre::OverlayManager();
  _fontManager    = new Ogre::FontManager();

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
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
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
  mUNLOCK;
}

void SceneGraphHandler::reset()
{
  cout << "SceneGraphHandler reset" << endl;
  mLOCK;
  _sceneGraph->reset();
  mUNLOCK;
}

void SceneGraphHandler::step()
{
  // cout << "SceneGraphHandler step" << endl;
  mLOCK;
  _sceneGraph->update();
  _root->renderOneFrame();
  mUNLOCK;
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
  cout << "SceneGraphHandler initialise" << endl;
  if(_sceneGraphInitialised == true) return;
  mLOCK;
  _sceneManager->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
  _rootNode   = _sceneManager->getRootSceneNode()->createChildSceneNode();
  _sceneGraph = new SceneGraph(_rootNode, _sceneManager);

  _coloredTextAreaOverlayElementFactory = new ColoredTextAreaOverlayElementFactory();
  Ogre::OverlayManager::getSingleton().addOverlayElementFactory(_coloredTextAreaOverlayElementFactory);
  _sceneGraphInitialised = true;
  mUNLOCK;
}
