#ifndef __SCENE_GRAPH_LIGHT_SOURCE_NODE_H__
#define __SCENE_GRAPH_LIGHT_SOURCE_NODE_H__

#include <yars/configuration/data/DataPointLightSource.h>
#include <yars/view/gui/SceneGraphObjectNode.h>

class SceneGraphLightSourceNode : public SceneGraphObjectNode
{
  public:
    SceneGraphLightSourceNode(DataPointLightSource*, Ogre::SceneNode*, Ogre::SceneManager*);
    void update();
    void reset() { };

  private:
    void __topCap();
    void __bottomCap();

    DataPointLightSource *_data;
    Ogre::ManualObject *_manual;
};

#endif // __SCENE_GRAPH_LIGHT_SOURCE_NODE_H__

