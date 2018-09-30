#ifndef __SCENE_GRAPH_OBJECT_NODE_H__
#define __SCENE_GRAPH_OBJECT_NODE_H__

#include <yars/types/Pose.h>
#include <yars/configuration/data/DataObject.h>
#include <yars/defines/types.h>

#include <OGRE/Ogre.h>

class SceneGraphObjectNode
{
public:
  SceneGraphObjectNode(Ogre::SceneNode *root, Ogre::SceneManager *sceneManager);
  virtual ~SceneGraphObjectNode();

  virtual void update() = 0;
  virtual void reset() = 0;

  Ogre::SceneNode *node();

protected:
  void setPose(DataObject *data);

  Ogre::SceneNode *_root;
  Ogre::SceneNode *_node;
  Ogre::SceneManager *_sceneManager;
  Ogre::Entity *_entity;
};

#endif // __SCENE_GRAPH_OBJECT_NODE_H__
