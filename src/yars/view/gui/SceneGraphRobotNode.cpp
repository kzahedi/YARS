#include "SceneGraphRobotNode.h"

#include <yars/view/gui/SceneGraphObjectFactory.h>
#include <yars/view/gui/SceneGraphJointAxisVisualisationNode.h>

#include <yars/util/macros.h>
#include <yars/configuration/YarsConfiguration.h>

SceneGraphRobotNode::SceneGraphRobotNode(
    DataRobot *robot, Ogre::SceneNode *root, Ogre::SceneManager *sm)
{
  for(DataObjects::iterator g = robot->g_begin(); g != robot->g_end(); g++)
  {
    SceneGraphObjectNode *objectNode = SceneGraphObjectFactory::create(*g, root, sm);
    if(objectNode != NULL)
    {
      _objects.push_back(objectNode);
      for(std::vector<DataSensor*>::iterator s = (*g)->s_begin(); s != (*g)->s_end(); s++)
      {
        switch((*s)->type())
        {
          case DATA_GENERIC_PROXIMITY_SENSOR:
            __createProximitySensor((DataGenericProximitySensor*)(*s), objectNode->node(), sm);
            break;
          case DATA_GENERIC_LIGHT_DEPENDENT_RESISTOR_SENSOR:
            __createLDRSensor((DataGenericLightDependentResistorSensor*)(*s), objectNode->node(), sm);
            break;
        }
      }
    }
  }

  if(Data::instance()->current()->screens()->visualiseJoints())
  {
    for(std::vector<DataActuator*>::iterator a = robot->a_begin(); a != robot->a_end(); a++)
    {
      if((*a)->type() != DATA_ACTUATOR_FIXED)
      {
        SceneGraphJointAxisVisualisationNode *actuator =
          new SceneGraphJointAxisVisualisationNode(*a, root, sm);
        _actuators.push_back(actuator);
      }
    }
  }
}

void SceneGraphRobotNode::__createProximitySensor(DataGenericProximitySensor* data, Ogre::SceneNode* node, Ogre::SceneManager* sm)
{
  SceneGraphProximitySensor *sensor = new SceneGraphProximitySensor(data, node, sm);
  _sensors.push_back(sensor);
}

void SceneGraphRobotNode::__createLDRSensor(DataGenericLightDependentResistorSensor* data, Ogre::SceneNode* node, Ogre::SceneManager* sm)
{
  SceneGraphLDRSensor *sensor = new SceneGraphLDRSensor(data, node, sm);
  _sensors.push_back(sensor);
}

SceneGraphRobotNode::~SceneGraphRobotNode()
{
  // FOREACH(SceneGraphObjectNode*, o, _objects) delete *o;
  // _objects.clear();
}

void SceneGraphRobotNode::update()
{
  FOREACH(SceneGraphObjectNode*, o, _objects)   if(*o != NULL) (*o)->update();
  FOREACH(SceneGraphObjectNode*, o, _sensors)   if(*o != NULL) (*o)->update();
  FOREACH(SceneGraphObjectNode*, o, _actuators) if(*o != NULL) (*o)->update();
}

void SceneGraphRobotNode::reset()
{
  FOREACH(SceneGraphObjectNode*, o, _objects)   if(*o != NULL) (*o)->reset();
  FOREACH(SceneGraphObjectNode*, o, _sensors)   if(*o != NULL) (*o)->reset();
  FOREACH(SceneGraphObjectNode*, o, _actuators) if(*o != NULL) (*o)->reset();
}

