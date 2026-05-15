#include "SceneGraphRobotNode.h"

#include <yars/view/gui/SceneGraphObjectFactory.h>
#include <yars/view/gui/SceneGraphJointAxisVisualisationNode.h>
#include <yars/view/gui/SceneGraphMuscleNode.h>

#include <yars/util/macros.h>
#include <yars/configuration/YarsConfiguration.h>

SceneGraphRobotNode::SceneGraphRobotNode(
    DataRobot *robot, Ogre::SceneNode *root, Ogre::SceneManager *sm)
{
  for (auto g = robot->g_begin(); g != robot->g_end(); ++g)
  {
    SceneGraphObjectNode *objectNode = SceneGraphObjectFactory::create(*g, root, sm);
    if (objectNode != nullptr)
    {
      Ogre::SceneNode *ogreNode = objectNode->node();
      _objects.emplace_back(objectNode);
      for (auto s = (*g)->s_begin(); s != (*g)->s_end(); ++s)
      {
        switch ((*s)->type())
        {
        case DATA_GENERIC_PROXIMITY_SENSOR:
          __createProximitySensor((DataGenericProximitySensor *)(*s), ogreNode, sm);
          break;
        case DATA_GENERIC_LIGHT_DEPENDENT_RESISTOR_SENSOR:
          __createLDRSensor((DataGenericLightDependentResistorSensor *)(*s), ogreNode, sm);
          break;
        }
      }
    }
  }

  // visualise muscles
  for (auto a = robot->a_begin(); a != robot->a_end(); ++a)
  {
    if ((*a)->type() == DATA_ACTUATOR_MUSCLE)
    {
      DataMuscleActuator *am = (DataMuscleActuator *)(*a);
      if (am->useMuscleVisualisation())
      {
        _actuators.emplace_back(new SceneGraphMuscleNode(am, root, sm));
      }
    }
  }

  if (Data::instance()->current()->screens()->visualiseJoints())
  {
    for (auto a = robot->a_begin(); a != robot->a_end(); ++a)
    {
      if ((*a)->type() != DATA_ACTUATOR_FIXED)
      {
        _actuators.emplace_back(new SceneGraphJointAxisVisualisationNode(*a, root, sm));
      }
    }
  }
}

void SceneGraphRobotNode::__createProximitySensor(DataGenericProximitySensor *data, Ogre::SceneNode *node, Ogre::SceneManager *sm)
{
  _sensors.emplace_back(new SceneGraphProximitySensor(data, node, sm));
}

void SceneGraphRobotNode::__createLDRSensor(DataGenericLightDependentResistorSensor *data, Ogre::SceneNode *node, Ogre::SceneManager *sm)
{
  _sensors.emplace_back(new SceneGraphLDRSensor(data, node, sm));
}

void SceneGraphRobotNode::update()
{
  for (auto &o : _objects)   if (o) o->update();
  for (auto &o : _sensors)   if (o) o->update();
  for (auto &o : _actuators) if (o) o->update();
}

void SceneGraphRobotNode::reset()
{
  for (auto &o : _objects)   if (o) o->reset();
  for (auto &o : _sensors)   if (o) o->reset();
  for (auto &o : _actuators) if (o) o->reset();
}
