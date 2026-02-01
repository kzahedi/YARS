#ifndef __SCENE_GRAPH_ENVIRONMENT_NODE_H__
#define __SCENE_GRAPH_ENVIRONMENT_NODE_H__

#include <yars/configuration/data/DataEnvironment.h>
#include <yars/view/gui/SceneGraphObjectNode.h>

#include <OGRE/Ogre.h>

#include <vector>
#include <memory>

using namespace std;

class SceneGraphEnvironmentNode : public SceneGraphObjectNode
{
public:
  SceneGraphEnvironmentNode(DataEnvironment *, Ogre::SceneNode *, Ogre::SceneManager *);
  ~SceneGraphEnvironmentNode() = default;

  void update();
  void reset(){};

private:
  DataEnvironment *_data;
  Ogre::SceneNode *_meshNode;
  std::vector<std::unique_ptr<SceneGraphObjectNode>> _lightSources;
  std::vector<std::unique_ptr<SceneGraphObjectNode>> _objects;
  std::vector<std::unique_ptr<SceneGraphObjectNode>> _meshes;
  std::vector<Ogre::SceneNode *> _meshNodes;
  vector<Ogre::Entity *> _entities;
  Ogre::TextureUnitState *_groundTextureUnitState;
  double _x;
  double _y;
  double _dx;
  double _dy;
  double _sx;
  double _sy;
};

#endif // __SCENE_GRAPH_ENVIRONMENT_NODE_H__
