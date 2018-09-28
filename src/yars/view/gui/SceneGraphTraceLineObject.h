#ifndef __SCENE_GRAPH_TRACE_LINE_OBJECT_H__
#define __SCENE_GRAPH_TRACE_LINE_OBJECT_H__

#include <yars/view/gui/SceneGraphObjectNode.h>
#include <yars/configuration/data/DataTraceLine.h>
#include <yars/types/Colour.h>

#include <Ogre.h>

class SceneGraphTraceLineObject : public SceneGraphObjectNode
{
public:
  SceneGraphTraceLineObject(DataTraceLine *trace,
                            Ogre::SceneNode *root,
                            Ogre::SceneManager *sceneManager,
                            int index);
  virtual ~SceneGraphTraceLineObject();

  void update();
  void reset();

private:
  DataTraceLine *_trace;
  Ogre::RibbonTrail *_trail;
  Ogre::BillboardSet *_billboard;
  Ogre::ParticleSystem *_particles;
  bool _reset;

  // DynamicLines       *_pLines;
};

#endif // __SCENE_GRAPH_TRACE_LINE_OBJECT_H__
