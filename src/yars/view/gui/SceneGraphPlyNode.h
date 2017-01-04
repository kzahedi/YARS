#ifndef __SCENE_GRAPH_PLY_NODE_H__
#define __SCENE_GRAPH_PLY_NODE_H__

#include <yars/configuration/data/DataPly.h>
#include <yars/view/gui/SceneGraphObjectNode.h>

class SceneGraphPlyNode : public SceneGraphObjectNode
{
  public:
    SceneGraphPlyNode(DataPly*, Ogre::SceneNode*, Ogre::SceneManager*);
    void update();
    void reset() { update(); };

  private:

    DataPly                 *_data;
    vector<Ogre::SceneNode*> _meshes;
    vector<Ogre::Entity*>    _entities;
    Ogre::ManualObject      *_manual;
};

#endif // __SCENE_GRAPH_PLY_NODE_H__
