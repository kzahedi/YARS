#ifndef __SCENE_GRAPH_H__
#define __SCENE_GRAPH_H__

#include <OGRE/Ogre.h>

#include <vector>

using namespace std;

#include "view/gui/SceneGraphRobotNode.h"
#include "view/gui/SceneGraphEnvironmentNode.h"
#include "view/gui/SceneGraphTraceLineObject.h"

class SceneGraph
{
  public:
    SceneGraph(Ogre::SceneNode *root, Ogre::SceneManager *sm);

    void update();
    void reset();

  private:
    void __createRobotsNodes();
    void __createEnvironmentNode();
    void __createTraces();

    Ogre::SceneManager                     *_sceneManager;
    Ogre::SceneNode                        *_root;
    std::vector<SceneGraphRobotNode*>     _robots;
    SceneGraphEnvironmentNode              *_environment;
    std::vector<SceneGraphTraceLineObject*> _traceLines;
};

#endif // __SCENE_GRAPH_H__
