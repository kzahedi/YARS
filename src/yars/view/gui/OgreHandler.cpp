#include "view/gui/OgreHandler.h"
#include "configuration/data/Data.h"
#include "util/Directories.h"

// #include <Overlay/OgreOverlaySystem.h>

OgreHandler* OgreHandler::_me = NULL;

OgreHandler* OgreHandler::instance()
{
  if(_me == NULL) _me = new OgreHandler();
  return _me;
}

OgreHandler::OgreHandler()
{
  Ogre::LogManager * lm = new Ogre::LogManager();
  lm->createLog("ogre.log", true, false, false); // create silent logging
  // TODO load resources.cfg
  _root = new Ogre::Root( "plugins.cfg", "ogre.cfg", ""); // no log file created here (see 1 line above)
  // _root = new Ogre::Root("","",""); // no log file created here (see 1 line above)

  // _GLPlugin = new Ogre::GLPlugin();
  // _GLPlugin->install();

  // _particlePlugin = new Ogre::ParticleFXPlugin();
  // _particlePlugin->install();

  if ( _root->getAvailableRenderers().size() != 1 )
  {
    OGRE_EXCEPT( Ogre::Exception::ERR_INTERNAL_ERROR, "Failed to initialize RenderSystem_GL", "main" );
  }
  // _root->setRenderSystem( _root->getAvailableRenderers()[0] );

  // initialize without creating window
  _root->getRenderSystem()->setConfigOption( "Full Screen", "No" );
  _root->saveConfig();

  _window = _root->initialise( false );
  // _window->removeAllViewports();
  _sceneManager = _root->createSceneManager(Ogre::ST_EXTERIOR_CLOSE, "Default SceneManager");
}

void OgreHandler::setupSceneManager()
{
  cout << "2000" << endl;
  // Ogre::OverlaySystem *mOverlaySystem = new Ogre::OverlaySystem();
  cout << "2001" << endl;
  // _sceneManager->addRenderQueueListener(mOverlaySystem);
  cout << "2002" << endl;

  // Load resource paths from config file
  if(Directories::doesFileExist("resources.cfg"))
  {
  cout << "2003" << endl;
    Ogre::ConfigFile cf;
  cout << "2004" << endl;
    cf.load("resources.cfg");
  cout << "2005" << endl;

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
  cout << "2006" << endl;

    Ogre::String secName, typeName, archName;
  cout << "2007" << endl;
    while (seci.hasMoreElements())
    {
  cout << "2008" << endl;
      secName = seci.peekNextKey();
  cout << "2009" << endl;
      Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
  cout << "2010" << endl;
      Ogre::ConfigFile::SettingsMultiMap::iterator i;
  cout << "2011" << endl;
      for (i = settings->begin(); i != settings->end(); ++i)
      {
  cout << "2012" << endl;
        typeName = i->first;
  cout << "2013" << endl;
        archName = i->second;
  cout << "2014" << endl;
        Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
            archName, typeName, secName);
  cout << "2015" << endl;
      }
    }
  }

  // Ogre::ResourceGroupManager::getSingleton().addResourceLocation("materials", "FileSystem");
  // Ogre::ResourceGroupManager::getSingleton().addResourceLocation("fonts",     "FileSystem");
  // Ogre::ResourceGroupManager::getSingleton().addResourceLocation("particles", "FileSystem");
  // Ogre::ResourceGroupManager::getSingleton().addResourceLocation("meshes",    "FileSystem");
  cout << "2016" << endl;
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
  cout << "2017" << endl;

  _sceneManager->setSkyDome(true, Data::instance()->current()->screens()->sky(), 20, 10);
  cout << "2018" << endl;
  // _sceneManager->setSkyBox(true, "YARS/SkyBox", 100000.0);

  _sceneManager->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
  cout << "2019" << endl;
  Ogre::SceneNode *node = _sceneManager->getRootSceneNode()->createChildSceneNode("lightNode");
  cout << "2020" << endl;
  Ogre::Light* lightSun = _sceneManager->createLight("sun");
  cout << "2021" << endl;
  lightSun->setPosition(75,150,75);
  cout << "2022" << endl;
  lightSun->setSpecularColour(0.25, 0.25, 0.25);
  cout << "2023" << endl;
  node->attachObject(lightSun);
  cout << "2024" << endl;

  //lightSun->setType(Ogre::Light::LT_DIRECTIONAL);
  //lightSun->setPosition(25,25,100);
  //lightSun->setDiffuseColour(Ogre::ColourValue(0.8, 0.8, 0.8));
  //lightSun->setSpecularColour(1.0, 1.0, 1.0);
  //lightSun->setDirection(Ogre::Vector3(-1, -1, 0));

  // Ogre::Light* specularSun = _sceneManager->createLight("spec sun");
  // specularSun->setPosition(75,150,75);
  // specularSun->setDiffuseColour(0.0, 0.0, 0.0);
  // specularSun->setSpecularColour(0.5, 0.5, 0.5);
  // node->attachObject(specularSun);

  _rootNode   = _sceneManager->getRootSceneNode()->createChildSceneNode();
  cout << "2025" << endl;
  _sceneGraph = new SceneGraph(_rootNode, _sceneManager);
  cout << "2026" << endl;


  // _textOverlay  = new TextOverlay("Legend Text Overlay");
  // Ogre::Real x = 10;
  // Ogre::Real y = 10;
  // _textOverlay->addTextBox("legend",
      // "^0YARS, Zahedi et al.", x, y, 150, 20,
      // Ogre::ColourValue(75.0/255.0, 117.0/255.0, 148.0/255.0,1.0f),
      // "Legend", "20");

// #ifdef USE_SHADOW_MAPS
  // _sceneManager->setShadowTexturePixelFormat(Ogre::PF_FLOAT16_R);
  // _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE);
  // _sceneManager->setShadowTextureSelfShadow(true);
  // _sceneManager->setShadowTextureCasterMaterial("ShadowCaster");
  // _sceneManager->setShadowTextureReceiverMaterial("ShadowReceiver");
// #else // USE_SHADOW_MAPS
  _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
  cout << "2027" << endl;
//#endif // USE_SHADOW_MAPS

  // _sceneManager->setShadowTextureSelfShadow(true);
  // _sceneManager->setShadowTextureCasterMaterial("DepthShadowmap/Caster/Float");
  // _sceneManager->setShadowTextureReceiverMaterial("Ogre/DepthShadowmap/Caster/Float");
  // _sceneManager->setShadowTexturePixelFormat(Ogre::PF_FLOAT32_R);
  // _sceneManager->setShadowCasterRenderBackFaces(false);
  // _sceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
}

Ogre::SceneManager* OgreHandler::getSceneManager()
{
  return _sceneManager;
}

void OgreHandler::reset()
{
  _sceneGraph->reset();
}

void OgreHandler::step()
{
  // cout << "OgreHandler 0" << endl;
  _sceneGraph->update();
  // cout << "OgreHandler 1" << endl;
  _root->renderOneFrame();
  // cout << "OgreHandler 2" << endl;
}

Ogre::Root* OgreHandler::root()
{
  return _root;
}

