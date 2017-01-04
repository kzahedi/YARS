#ifndef __SCENE_GRAPH_H__
#define __SCENE_GRAPH_H__

#include <yars/view/gui/SceneGraphRobotNode.h>
#include <yars/view/gui/SceneGraphEnvironmentNode.h>
#include <yars/view/gui/SceneGraphTraceLineObject.h>

#include <OGRE/Ogre.h>

#include <vector>

using namespace std;

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
