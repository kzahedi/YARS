#ifndef __SCENE_GRAPH_JOINT_AXIS_VISUALISATION_NODE_H__
#define __SCENE_GRAPH_JOINT_AXIS_VISUALISATION_NODE_H__

#include "SceneGraphObjectNode.h"
#include "configuration/data/DataActuator.h"
#include "types/RHDimension.h"

#include <vector>

class SceneGraphJointAxisVisualisationNode : public SceneGraphObjectNode
{
  public:
    SceneGraphJointAxisVisualisationNode(DataActuator*, Ogre::SceneNode*, Ogre::SceneManager*);
    void update();
    void reset() { };

  private:
    void __bottomCap(Ogre::ManualObject *manual);
    void __topCap(Ogre::ManualObject *manual);
    void __body(Ogre::ManualObject *manual);
    Ogre::Quaternion __calculateAxisX(P3D axis);

    DataActuator       *_data;
    Ogre::ManualObject *_manualX;
    Ogre::ManualObject *_manualY;
    Ogre::ManualObject *_manualZ;
    RHDimension         _dimension;
};

#endif // __SCENE_GRAPH_JOINT_AXIS_VISUALISATION_NODE_H__


