#ifndef __SCENE_GRAPH_H__
#define __SCENE_GRAPH_H__

#include <yars/view/gui/SceneGraphRobotNode.h>
#include <yars/view/gui/SceneGraphEnvironmentNode.h>
#include <yars/view/gui/SceneGraphTraceLineObject.h>

#include <OGRE/Ogre.h>

#include <vector>
#include <memory>

using namespace std;

class SceneGraph
{
public:
  SceneGraph(Ogre::SceneNode *root, Ogre::SceneManager *sm);
  ~SceneGraph() = default;

  void update();
  void reset();

private:
  void __createRobotsNodes();
  void __createEnvironmentNode();
  void __createTraces();

  Ogre::SceneManager *_sceneManager;
  Ogre::SceneNode *_root;
  std::vector<std::unique_ptr<SceneGraphRobotNode>> _robots;
  std::unique_ptr<SceneGraphEnvironmentNode> _environment;
  std::vector<std::unique_ptr<SceneGraphTraceLineObject>> _traceLines;
};

#endif // __SCENE_GRAPH_H__
