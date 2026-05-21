#ifndef __OGRE_HANDLER_H__
#define __OGRE_HANDLER_H__

#include <yars/view/gui/SceneGraph.h>
#include <yars/view/gui/TextOverlay.h>

#include <OGRE/Ogre.h>
#include <OGRE/RenderSystems/GL3Plus/OgreGL3PlusPlugin.h>
#include <OGRE/Plugins/ParticleFX/OgreParticleFXPlugin.h>
#include <OGRE/RTShaderSystem/OgreRTShaderSystem.h>
#include <OGRE/Plugins/STBICodec/OgreSTBICodec.h>

#include <memory>

namespace yars {

class ShadowMapper;

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
  void __setupShadows();

  static OgreHandler *_me;

  Ogre::Root *_root;
  Ogre::SceneManager *_sceneManager;
  Ogre::SceneNode *_rootNode;
  SceneGraph *_sceneGraph;
  Ogre::RTShader::ShaderGenerator *_shaderGenerator;
  Ogre::MaterialManager::Listener *_materialListener{nullptr};
  std::unique_ptr<ShadowMapper> _shadowMapper;
  // TextOverlay        *_textOverlay;
};

} // namespace yars

#endif // __OGRE_HANDLER_H__
