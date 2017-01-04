#ifndef __SCENE_GRAPH_CAPPED_CYLINDER_NODE_H__
#define __SCENE_GRAPH_CAPPED_CYLINDER_NODE_H__

#include <yars/view/gui/SceneGraphObjectNode.h>
#include <yars/configuration/data/DataCapsule.h>

#include <vector>

class SceneGraphCapsuleNode : public SceneGraphObjectNode
{
  public:
    SceneGraphCapsuleNode(DataCapsule*, Ogre::SceneNode*, Ogre::SceneManager*);
    void update();
    void reset() { update(); };

  private:
    void __topCap();
    void __bottomCap();
    void __body();

    DataCapsule *_data;
    Ogre::ManualObject *_manual;
    RHDimension         _dimension;
};

#endif // __SCENE_GRAPH_CAPPED_CYLINDER_NODE_H__


