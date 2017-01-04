#include "OgreHandler.h"
#include <yars/configuration/data/Data.h>
#include <yars/util/Directories.h>

#if __APPLE__
#include <OgreOverlay/OgreOverlaySystem.h>
#else
#include <OGRE/Overlay/OgreOverlaySystem.h>
#include <OGRE/RenderSystems/GL/OgreGLPlugin.h>
#endif

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

#ifdef __APPLE__
  _root = new Ogre::Root("","",""); // no log file created here (see 1 line above)
#else // __APPLE__
  _root = new Ogre::Root("plugins.cfg","ogre.cfg",""); // no log file created here (see 1 line above)
#endif // __APPLE__


#ifdef __APPLE__
  _GLPlugin = new Ogre::GLPlugin();
  _GLPlugin->install();

  _particlePlugin = new Ogre::ParticleFXPlugin();
  _particlePlugin->install();
#endif // __APPLE__

  if ( _root->getAvailableRenderers().size() != 1 ) {
    OGRE_EXCEPT( Ogre::Exception::ERR_INTERNAL_ERROR, "Failed to initialize RenderSystem_GL", "main" );
  }
  _root->setRenderSystem( _root->getAvailableRenderers()[0] );
  _root->initialise( false );
  _sceneManager = _root->createSceneManager(Ogre::ST_GENERIC, "Default SceneManager");
}

void OgreHandler::setupSceneManager()
{
  Ogre::OverlaySystem *mOverlaySystem = new Ogre::OverlaySystem();
  _sceneManager->addRenderQueueListener(mOverlaySystem);

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

  // Ogre::ResourceGroupManager::getSingleton().addResourceLocation("materials", "FileSystem");
  // Ogre::ResourceGroupManager::getSingleton().addResourceLocation("fonts",     "FileSystem");
  // Ogre::ResourceGroupManager::getSingleton().addResourceLocation("particles", "FileSystem");
  // Ogre::ResourceGroupManager::getSingleton().addResourceLocation("meshes",    "FileSystem");
  Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();



  _sceneManager->setSkyDome(true, Data::instance()->current()->screens()->sky(), 20, 10);
  // _sceneManager->setSkyBox(true, "YARS/SkyBox", 100000.0);

  _sceneManager->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
  Ogre::SceneNode *node = _sceneManager->getRootSceneNode()->createChildSceneNode("lightNode");
  Ogre::Light* lightSun = _sceneManager->createLight("sun");
  lightSun->setPosition(75,150,75);
  lightSun->setSpecularColour(0.25, 0.25, 0.25);
  node->attachObject(lightSun);

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
  _sceneGraph = new SceneGraph(_rootNode, _sceneManager);


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

