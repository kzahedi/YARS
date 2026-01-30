#ifndef __OGRE_HANDLER_H__
#define __OGRE_HANDLER_H__

#include <yars/view/gui/SceneGraph.h>
#include <yars/view/gui/TextOverlay.h>

#include <OGRE/Ogre.h>
#include <OGRE/RenderSystems/GL3Plus/OgreGL3PlusPlugin.h>
#include <OGRE/Plugins/ParticleFX/OgreParticleFXPlugin.h>
#include <OGRE/Codec/OgreSTBICodec.h>
#include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>

class OgreHandler
{
public:
  static OgreHandler *instance();
  static void shutdown();

  Ogre::SceneManager *getSceneManager();
  Ogre::Root *root();

  void setupSceneManager();
  void setupResources();

  void step();
  void reset();

private:
  OgreHandler();
  ~OgreHandler();

  static OgreHandler *_me;

  Ogre::Root *_root;
  Ogre::SceneManager *_sceneManager;
  Ogre::SceneNode *_rootNode;
  SceneGraph *_sceneGraph;
  Ogre::GL3PlusPlugin *_GL3PlusPlugin;
  Ogre::ParticleFXPlugin *_particlePlugin;
  Ogre::RTShader::ShaderGenerator *_shaderGenerator;
  // TextOverlay        *_textOverlay;
};

#endif // __OGRE_HANDLER_H__
