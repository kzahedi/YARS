#ifndef __OGRE_HANDLER_H__
#define __OGRE_HANDLER_H__

#include <yars/view/gui/SceneGraph.h>
#include <yars/view/gui/TextOverlay.h>

#include <OGRE/Ogre.h>
#if __APPLE__
#include <RenderSystems/GL3Plus/OgreGL3PlusPlugin.h>
#include <Plugins/ParticleFX/OgreParticleFXPlugin.h>
#include <RTShaderSystem/OgreRTShaderSystem.h>
#else
#include <OGRE/RenderSystems/GL3Plus/OgreGL3PlusPlugin.h>
#include <OGRE/Plugins/ParticleFX/OgreParticleFXPlugin.h>
#include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>
#endif
// #include <OGRE/OgreStaticPluginLoader.h>

class OgreHandler
{
public:
  static OgreHandler *instance();
  ~OgreHandler();

  Ogre::SceneManager *getSceneManager();
  Ogre::Root *root();

  void setupSceneManager();
  void setupResources();

  void step();
  void reset();

private:
  OgreHandler();

  static OgreHandler *_me;

  Ogre::Root *_root;
  Ogre::SceneManager *_sceneManager;
  Ogre::SceneNode *_rootNode;
  SceneGraph *_sceneGraph;
  Ogre::GL3PlusPlugin *_GLPlugin;
  Ogre::ParticleFXPlugin *_particlePlugin;
  Ogre::RTShader::ShaderGenerator *_shaderGenerator;
  Ogre::MaterialManager::Listener *_materialListener{nullptr};
  // TextOverlay        *_textOverlay;
};

#endif // __OGRE_HANDLER_H__
