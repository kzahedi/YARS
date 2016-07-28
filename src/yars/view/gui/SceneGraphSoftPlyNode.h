#ifndef __SCENE_GRAPH_SOFT_PLY_NODE_H__
#define __SCENE_GRAPH_SOFT_PLY_NODE_H__

#include "configuration/data/DataPly.h"
#include "SceneGraphObjectNode.h"
#include "types/Vertex.h"


class SceneGraphSoftPlyNode : public SceneGraphObjectNode
{
  public:
    SceneGraphSoftPlyNode(DataPly*, Ogre::SceneNode*, Ogre::SceneManager*);
    void update();
    void reset() { update(); };

  private:

    DataPly                 *_data;
    vector<Ogre::SceneNode*> _meshes;
    vector<Ogre::Entity*>    _entities;
    Ogre::ManualObject      *_manual;
    Vertices                 _vertices;
    int                      _lastStep;
};

#endif // __SCENE_GRAPH_SOFT_PLY_NODE_H__
