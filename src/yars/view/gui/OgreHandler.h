#ifndef __OGRE_HANDLER_H__
#define __OGRE_HANDLER_H__

#include <yars/view/gui/SceneGraph.h>
#include <yars/view/gui/TextOverlay.h>


#include <OGRE/Ogre.h>
#if __APPLE__
#include <RenderSystem_GL/OgreGLPlugin.h>
#include <Plugin_ParticleFX/OgreParticleFXPlugin.h>
#else
#include <RenderSystems/GL/OgreGLPlugin.h>
#include <Plugins/ParticleFX/OgreParticleFXPlugin.h>
#endif
// #include <OGRE/OgreStaticPluginLoader.h>

class OgreHandler
{
  public:
    static OgreHandler* instance();

    Ogre::SceneManager* getSceneManager();
    Ogre::Root*         root();

    void setupSceneManager();
    void setupResources();

    void step();
    void reset();

  private:
    OgreHandler();

    static OgreHandler *_me;

    Ogre::Root             *_root;
    Ogre::SceneManager     *_sceneManager;
    Ogre::SceneNode        *_rootNode;
    SceneGraph             *_sceneGraph;
    Ogre::GLPlugin         *_GLPlugin;
    Ogre::ParticleFXPlugin *_particlePlugin;
    // TextOverlay        *_textOverlay;

};


#endif // __OGRE_HANDLER_H__
