#ifndef __SCENE_GRAPH_ROBOT_NODE_H__
#define __SCENE_GRAPH_ROBOT_NODE_H__

#include <yars/configuration/data/DataRobot.h>
#include <yars/view/gui/SceneGraphProximitySensor.h>
#include <yars/view/gui/SceneGraphLDRSensor.h>
#include <yars/view/gui/SceneGraphObjectNode.h>
#include <OGRE/Ogre.h>

#include <vector>

using namespace std;

class SceneGraphRobotNode
{
  public:
    SceneGraphRobotNode(DataRobot *robot, Ogre::SceneNode *root, Ogre::SceneManager *sm);
    ~SceneGraphRobotNode();

    void update();
    void reset();

  private:
    void __createProximitySensor(DataGenericProximitySensor*        data, Ogre::SceneNode* node, Ogre::SceneManager* sm);
    void __createLDRSensor(DataGenericLightDependentResistorSensor* data, Ogre::SceneNode* node, Ogre::SceneManager* sm);

    std::vector<SceneGraphObjectNode*> _objects;
    std::vector<SceneGraphObjectNode*> _sensors;
    std::vector<SceneGraphObjectNode*> _actuators;
    std::vector<string>                _osd;

};

#endif // __SCENE_GRAPH_ROBOT_NODE_H__
