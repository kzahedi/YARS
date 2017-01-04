#include "SceneGraph.h"
#include <yars/configuration/data/Data.h>

#include <sstream>

#define SHOW_XYZ \
  for(int i = 1; i < 100; i++)\
  {\
    stringstream oss;\
    oss << "sphere x " << i << endl;\
    Ogre::SceneNode* mSceneNode = _root->createChildSceneNode();\
    Ogre::Entity*    mEntity    = _sceneManager->createEntity(oss.str().c_str(),\
        Ogre::SceneManager::PT_SPHERE);\
    mEntity->setMaterialName("YARS/Red");\
    mSceneNode->attachObject(mEntity);\
    mSceneNode->setPosition(Ogre::Vector3((float)i * 0.05, 0, 0));\
    mSceneNode->setScale(Ogre::Vector3(0.001, 0.001, 0.001));\
  }\
  for(int i = 1; i < 100; i++)\
  {\
    stringstream oss;\
    oss << "sphere y " << i << endl;\
    Ogre::SceneNode* mSceneNode = _root->createChildSceneNode();\
    Ogre::Entity*    mEntity    = _sceneManager->createEntity(oss.str().c_str(),\
        Ogre::SceneManager::PT_SPHERE);\
    mEntity->setMaterialName("YARS/Green");\
    mSceneNode->attachObject(mEntity);\
    mSceneNode->setPosition(Ogre::Vector3(0, (float)i * 0.05, 0));\
    mSceneNode->setScale(Ogre::Vector3(0.001, 0.001, 0.001));\
  }\
  for(int i = 1; i < 100; i++)\
  {\
    stringstream oss;\
    oss << "sphere z " << i << endl;\
    Ogre::SceneNode* mSceneNode = _root->createChildSceneNode();\
    Ogre::Entity*    mEntity    = _sceneManager->createEntity(oss.str().c_str(),\
        Ogre::SceneManager::PT_SPHERE);\
    mEntity->setMaterialName("YARS/Blue");\
    mSceneNode->attachObject(mEntity);\
    mSceneNode->setPosition(Ogre::Vector3(0, 0, (float)i * 0.05));\
    mSceneNode->setScale(Ogre::Vector3(0.001, 0.001, 0.001));\
  }



SceneGraph::SceneGraph(Ogre::SceneNode *root, Ogre::SceneManager* sm)
{
  _root         = root;
  _sceneManager = sm;
  ::Quaternion q(P3D(-M_PI_2, 0.0, 0.0));
  _root->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z));

  __createRobotsNodes();
  __createEnvironmentNode();
  __createTraces();

  // SHOW_XYZ;

  // Ogre::SceneNode* mSceneNode2 = root->createChildSceneNode();
  // Ogre::Entity*    mEntity2    = _sceneManager->createEntity("mySphere2", Ogre::SceneManager::PT_SPHERE);
  // mEntity2->setMaterialName("YARS/SkinPattern");
  // mSceneNode2->attachObject(mEntity2);
  // mSceneNode2->setPosition(Ogre::Vector3(0,1,0));
  // mSceneNode2->setScale(Ogre::Vector3(1.1,0.1,1.1)); // Radius, in theory.
}

void SceneGraph::__createRobotsNodes()
{
  DataRobots *robots = Data::instance()->current()->robots();

  FOREACHP(DataRobot*, i, robots)
  {
    SceneGraphRobotNode *robot = new SceneGraphRobotNode(*i, _root, _sceneManager);
    _robots.push_back(robot);
  }
}

void SceneGraph::__createEnvironmentNode()
{
  DataEnvironment *environment = Data::instance()->current()->environment();
  _environment = new SceneGraphEnvironmentNode(environment, _root, _sceneManager);
}

void SceneGraph::update()
{
  // cout << "SceneGraph::update 0" << endl;
  _environment->update();
  // cout << "SceneGraph::update 1" << endl;
  FOREACH(SceneGraphRobotNode*,       m, _robots)     (*m)->update();
  // cout << "SceneGraph::update 2" << endl;
  FOREACH(SceneGraphTraceLineObject*, t, _traceLines) (*t)->update();
  // cout << "SceneGraph::update 3" << endl;
}

void SceneGraph::__createTraces()
{
  DataTraces *traces = Data::instance()->current()->traces();
  if(traces == NULL) return;

  int index = 0;
  for(std::vector<DataTraceLine*>::iterator l = traces->l_begin(); l != traces->l_end(); l++)
  {
    SceneGraphTraceLineObject *traceLine = new SceneGraphTraceLineObject((*l), _root, _sceneManager, index);
    _traceLines.push_back(traceLine);
    index++;
  }

  // for(std::vector<DataTracePoint*>::iterator p = traces->p_begin(); p != traces->p_end(); p++)
  // {
    // SceneGraphTracePointObject *tracePoint = new SceneGraphTracePointObject((*l));
    // _n_scene->addChild(tracePoint->node());
    // _svp->editMFExcludeNodes()->push_back(tracePoint->node()); // trace lines should not throw shadows
    // _tracePoints.push_back(tracePoint);
  // }

}

void SceneGraph::reset()
{
  FOREACH(SceneGraphRobotNode*,       m, _robots)     (*m)->reset();
  FOREACH(SceneGraphTraceLineObject*, t, _traceLines) (*t)->reset();
  _environment->reset();
}
