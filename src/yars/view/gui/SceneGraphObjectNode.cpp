#include "SceneGraphObjectNode.h"

#include <yars/configuration/YarsConfiguration.h>

#include <iostream>

using namespace std;

SceneGraphObjectNode::SceneGraphObjectNode(Ogre::SceneNode* root, Ogre::SceneManager* sceneManager)
{
  _root         = root;
  _sceneManager = sceneManager;
}

SceneGraphObjectNode::~SceneGraphObjectNode()
{ }

void SceneGraphObjectNode::setPose(DataObject *data)
{ }

Ogre::SceneNode* SceneGraphObjectNode::node()
{
  return _node;
}
