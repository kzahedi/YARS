#ifndef __SCENE_GRAPH_MUSCLE_NODE_H__
#define __SCENE_GRAPH_MUSCLE_NODE_H__

#include <yars/view/gui/SceneGraphObjectNode.h>
#include <yars/configuration/data/DataMuscleActuator.h>

#include <vector>

class SceneGraphMuscleNode : public SceneGraphObjectNode
{
public:
  SceneGraphMuscleNode(DataMuscleActuator *, Ogre::SceneNode *, Ogre::SceneManager *);
  void update();
  void reset() { update(); };

private:
  void __bottomCap();
  void __topCap();
  void __body();
  double __getRadius(int);

  DataMuscleActuator *_data;
  Ogre::ManualObject *_manual;
  double _width;
  double _height;
  double _radiusSrc;
  double _radiusDst;
  std::vector<int> _topIndices;
};

#endif // __SCENE_GRAPH_CYLINDER_NODE_H__
