#ifndef __SCENE_GRAPH_BOX_NODE_H__
#define __SCENE_GRAPH_BOX_NODE_H__

#include <yars/view/gui/SceneGraphObjectNode.h>
#include <yars/configuration/data/DataBox.h>

class SceneGraphBoxNode : public SceneGraphObjectNode
{
  public:
    SceneGraphBoxNode(DataBox *box, Ogre::SceneNode*, Ogre::SceneManager*);
    void update();
    void reset() { update(); };

  private:

    DataBox            *_data;
    Ogre::ManualObject *_manual;
};

#endif // __SCENE_GRAPH_BOX_NODE_H__
