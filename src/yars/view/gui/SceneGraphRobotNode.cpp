#include "SceneGraphRobotNode.h"

#include <yars/view/gui/SceneGraphObjectFactory.h>
#include <yars/view/gui/SceneGraphJointAxisVisualisationNode.h>
#include <yars/view/gui/SceneGraphMuscleNode.h>

#include <yars/util/macros.h>
#include <yars/configuration/YarsConfiguration.h>

SceneGraphRobotNode::SceneGraphRobotNode(
    DataRobot *robot, Ogre::SceneNode *root, Ogre::SceneManager *sm)
{
  for (auto g = robot->g_begin(); g != robot->g_end(); g++)
  {
    std::unique_ptr<SceneGraphObjectNode> objectNode(SceneGraphObjectFactory::create(*g, root, sm));
    if (objectNode != nullptr)
    {
      auto* nodePtr = objectNode.get();
      _objects.push_back(std::move(objectNode));
      for (auto s = (*g)->s_begin(); s != (*g)->s_end(); s++)
      {
        switch ((*s)->type())
        {
        case DATA_GENERIC_PROXIMITY_SENSOR:
          __createProximitySensor((DataGenericProximitySensor *)(*s), nodePtr->node(), sm);
          break;
        case DATA_GENERIC_LIGHT_DEPENDENT_RESISTOR_SENSOR:
          __createLDRSensor((DataGenericLightDependentResistorSensor *)(*s), nodePtr->node(), sm);
          break;
        }
      }
    }
  }

  // visualise muscles
  for (auto a = robot->a_begin(); a != robot->a_end(); a++)
  {
    if ((*a)->type() == DATA_ACTUATOR_MUSCLE)
    {
      DataMuscleActuator *am = (DataMuscleActuator *)(*a);
      if (am->useMuscleVisualisation())
      {
        _actuators.push_back(std::make_unique<SceneGraphMuscleNode>(am, root, sm));
      }
    }
  }

  if (Data::instance()->current()->screens()->visualiseJoints())
  {
    for (auto a = robot->a_begin(); a != robot->a_end(); a++)
    {
      if ((*a)->type() != DATA_ACTUATOR_FIXED)
      {
        _actuators.push_back(std::make_unique<SceneGraphJointAxisVisualisationNode>(*a, root, sm));
      }
    }
  }
}

void SceneGraphRobotNode::__createProximitySensor(DataGenericProximitySensor *data, Ogre::SceneNode *node, Ogre::SceneManager *sm)
{
  _sensors.push_back(std::make_unique<SceneGraphProximitySensor>(data, node, sm));
}

void SceneGraphRobotNode::__createLDRSensor(DataGenericLightDependentResistorSensor *data, Ogre::SceneNode *node, Ogre::SceneManager *sm)
{
  _sensors.push_back(std::make_unique<SceneGraphLDRSensor>(data, node, sm));
}

void SceneGraphRobotNode::update()
{
  for (auto& o : _objects)
    if (o) o->update();
  for (auto& o : _sensors)
    if (o) o->update();
  for (auto& o : _actuators)
    if (o) o->update();
}

void SceneGraphRobotNode::reset()
{
  for (auto& o : _objects)
    if (o) o->reset();
  for (auto& o : _sensors)
    if (o) o->reset();
  for (auto& o : _actuators)
    if (o) o->reset();
}
