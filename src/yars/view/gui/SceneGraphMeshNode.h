#ifndef __SCENE_GRAPH_MESH_NODE_H__
#define __SCENE_GRAPH_MESH_NODE_H__

#include "configuration/data/DataObject.h"
#include "SceneGraphObjectNode.h"

class SceneGraphMeshNode : public SceneGraphObjectNode
{
  public:
    SceneGraphMeshNode(DataObject *box, Ogre::SceneNode*, Ogre::SceneManager*);
    void update();
    // void reset() { update(); };
    void reset() { };

  private:

    DataObject      *_data;
    vector<Ogre::SceneNode*> _meshes;
    vector<Ogre::Entity*>    _entities;
};

#endif // __SCENE_GRAPH_MESH_NODE_H__

