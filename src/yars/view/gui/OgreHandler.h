#ifndef __OGRE_HANDLER_H__
#define __OGRE_HANDLER_H__

#include <OGRE/Ogre.h>
#include <OGRE/RenderSystems/GL/OgreGLPlugin.h>
#include <OGRE/Plugins/ParticleFX/OgreParticleFXPlugin.h>
// #include <OGRE/OgreStaticPluginLoader.h>

#include "view/gui/SceneGraph.h"
#include "view/gui/TextOverlay.h"

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

    Ogre::Root*         _root;
    Ogre::SceneManager* _sceneManager;
    Ogre::SceneNode*    _rootNode;
    SceneGraph*         _sceneGraph;
    Ogre::GLPlugin*     _GLPlugin;
    Ogre::ParticleFXPlugin*_particlePlugin;
    Ogre::RenderWindow* _window;
};


#endif // __OGRE_HANDLER_H__
