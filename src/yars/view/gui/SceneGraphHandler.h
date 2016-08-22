#ifndef __SCENE_GRAPH_HANDLER_H__
#define __SCENE_GRAPH_HANDLER_H__

#include "SceneGraph.h"
#include "ColoredTextAreaOverlayElementFactory.h"
#include "GuiMutex.h"

#include <OGRE/Ogre.h>

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

    ColoredTextAreaOverlayElementFactory* _coloredTextAreaOverlayElementFactory;
    Ogre::OverlayManager*                 _overlayManager;
    Ogre::FontManager*                    _fontManager;
};


#endif // __SCENE_GRAPH_HANDLER_H__
