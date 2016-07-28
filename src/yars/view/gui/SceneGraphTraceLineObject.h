#ifndef __SCENE_GRAPH_TRACE_LINE_OBJECT_H__
#define __SCENE_GRAPH_TRACE_LINE_OBJECT_H__

#include "SceneGraphObjectNode.h"

#include "configuration/data/DataTraceLine.h"

#include <OGRE/Ogre.h>

#include "types/Colour.h"


class SceneGraphTraceLineObject : public SceneGraphObjectNode
{
  public:
    SceneGraphTraceLineObject(DataTraceLine *trace,
        Ogre::SceneNode* root,
        Ogre::SceneManager* sceneManager,
        int index);
    virtual ~SceneGraphTraceLineObject();

    void update();
    void reset();

  private:
    DataTraceLine        *_trace;
    Ogre::RibbonTrail    *_trail;
    Ogre::BillboardSet   *_billboard;
    Ogre::ParticleSystem *_particles;
    bool _reset;

    // DynamicLines       *_pLines;
};

#endif // __SCENE_GRAPH_TRACE_LINE_OBJECT_H__


