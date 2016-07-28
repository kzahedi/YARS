#ifndef __SCENE_GRAPH_SPHERE_NODE_H__
#define __SCENE_GRAPH_SPHERE_NODE_H__

#include "configuration/data/DataSphere.h"
#include "SceneGraphObjectNode.h"

class SceneGraphSphereNode : public SceneGraphObjectNode
{
  public:
    SceneGraphSphereNode(DataSphere*, Ogre::SceneNode*, Ogre::SceneManager*);
    void update();
    void reset() { update(); };

  private:
    void __topCap();
    void __bottomCap();

    DataSphere *_data;
    Ogre::ManualObject *_manual;
};

#endif // __SCENE_GRAPH_SPHERE_NODE_H__

