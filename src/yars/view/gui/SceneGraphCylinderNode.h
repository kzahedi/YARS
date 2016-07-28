#ifndef __SCENE_GRAPH_CYLINDER_NODE_H__
#define __SCENE_GRAPH_CYLINDER_NODE_H__

#include "SceneGraphObjectNode.h"
#include "configuration/data/DataCylinder.h"

#include <vector>

class SceneGraphCylinderNode : public SceneGraphObjectNode
{
  public:
    SceneGraphCylinderNode(DataCylinder*, Ogre::SceneNode*, Ogre::SceneManager*);
    void update();
    void reset() { update(); };

  private:
    void __bottomCap();
    void __topCap();
    void __body();

    DataCylinder *_data;
    Ogre::ManualObject *_manual;
    RHDimension _dimension;
};

#endif // __SCENE_GRAPH_CYLINDER_NODE_H__


