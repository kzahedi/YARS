#ifndef __SCENE_GRAPH_ENVIRONMENT_NODE_H__
#define __SCENE_GRAPH_ENVIRONMENT_NODE_H__

#include <yars/configuration/data/DataEnvironment.h>
#include <yars/view/gui/SceneGraphObjectNode.h>

#include <Ogre.h>

#include <vector>

using namespace std;

class SceneGraphEnvironmentNode : public SceneGraphObjectNode
{
public:
  SceneGraphEnvironmentNode(DataEnvironment *, Ogre::SceneNode *, Ogre::SceneManager *);
  ~SceneGraphEnvironmentNode();

  void update();
  void reset(){};

private:
  DataEnvironment *_data;
  Ogre::SceneNode *_meshNode;
  std::vector<SceneGraphObjectNode *> _lightSources;
  std::vector<SceneGraphObjectNode *> _objects;
  std::vector<SceneGraphObjectNode *> _meshes;
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
