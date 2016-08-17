#ifndef __SCENE_GRAPH_HANDLER_H__
#define __SCENE_GRAPH_HANDLER_H__

#include <OGRE/Ogre.h>
#include "SceneGraph.h"

class SceneGraphHandler
{
  public:
    static SceneGraphHandler* instance();

    Ogre::SceneManager* getSceneManager();

    void setupSceneManager();
    void setupResources();

    void step();
    void reset();
    void initialise();

    Ogre::Root*         root();
    Ogre::SceneManager* sceneManager();

  private:
    SceneGraphHandler();

    static SceneGraphHandler *_me;

    Ogre::Root*         _root;
    Ogre::SceneManager* _sceneManager;
    Ogre::SceneNode*    _rootNode;
    SceneGraph*         _sceneGraph;
    bool                _sceneGraphInitialised;
};


#endif // __SCENE_GRAPH_HANDLER_H__
