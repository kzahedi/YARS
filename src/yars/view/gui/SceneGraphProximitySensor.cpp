#include "SceneGraphProximitySensor.h"

#include <yars/types/Colour.h>

#include <OGRE/Ogre.h>

SceneGraphProximitySensor::SceneGraphProximitySensor(DataGenericProximitySensor *data,
    Ogre::SceneNode* r, Ogre::SceneManager* sm)
: SceneGraphObjectNode(r,sm)
{
  // 5 Rays
  _data    = data;
  P3D position = _data->pose().position;
  _points.resize(5);

  _node    = _root->createChildSceneNode();
  _manual  = _sceneManager->createManualObject(_data->name());
  _node->setPosition(Ogre::Vector3(position.x, position.y, position.z));

  ::Quaternion q(_data->pose().orientation);
  _node->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z));

  P2D oa = _data->openingAngles();
  _quaternions.push_back(::Quaternion(P3D(  0.0,   0.0, 0.0)));
  _quaternions.push_back(::Quaternion(P3D(-oa.x,   0.0, 0.0)));
  _quaternions.push_back(::Quaternion(P3D(  0.0,  oa.y, 0.0)));
  _quaternions.push_back(::Quaternion(P3D( oa.x,   0.0, 0.0)));
  _quaternions.push_back(::Quaternion(P3D(  0.0, -oa.y, 0.0)));

  for(int i = 0; i < 5; i++) __setRay(i, _data->distance());

  __rays();
  __cap();
  __body();

  _manual->setMaterialName(0, "YARS/ProximitySensor");
  _manual->setMaterialName(1, "YARS/ProximitySensor");
  _manual->setMaterialName(2, "YARS/ProximitySensor");

  _node->attachObject(_manual);
}

SceneGraphProximitySensor::~SceneGraphProximitySensor()
{
}

void SceneGraphProximitySensor::update()
{
  yReal min = MIN(_data->distance(), _data->getMeasuredDistance());
  for(int i = 0; i < 5; i++) __setRay(i, min);
  __updateContainers();
}

void SceneGraphProximitySensor::__updateRayColour(int index, bool collided)
{
  // if(collided)
  // {
    // _rayColours[2 * index].setValuesRGBA(1.0f, 0.0f, 0.0f, 0.9);
    // _rayColours[2 * index + 1].setValuesRGBA(1.0f, 0.0f, 0.0f, 0.9);
  // }
  // else
  // {
    // _rayColours[2 * index].setValuesRGBA(1.0f, 1.0f, 1.0f, 0.75f);
    // _rayColours[2 * index + 1].setValuesRGBA(1.0f, 1.0f, 1.0f, 0.75f);
  // }
}

void SceneGraphProximitySensor::__setRay(int index, yReal length)
{
  P3D ray(0, 0, length);
  ray *= _quaternions[index];
  _points[index] = ray;
}

void SceneGraphProximitySensor::__updateContainers()
{
  _manual->beginUpdate(0);
  for(int i = 0; i < 5; i++)
  {
    _manual->position(0.0,          0.0,          0.0);
    _manual->position(_points[i].x, _points[i].y, _points[i].z);
  }
  _manual->end();

  _manual->beginUpdate(1);
  _manual->position(_points[0].x, _points[0].y, _points[0].z);
  _manual->position(_points[4].x, _points[4].y, _points[4].z);
  _manual->position(_points[3].x, _points[3].y, _points[3].z);
  _manual->position(_points[2].x, _points[2].y, _points[2].z);
  _manual->position(_points[1].x, _points[1].y, _points[1].z);
  _manual->position(_points[4].x, _points[4].y, _points[4].z);
  _manual->end();

  _manual->beginUpdate(2);
  _manual->position(0.0, 0.0, 0.0);
  _manual->position(_points[1].x, _points[1].y, _points[1].z);
  _manual->position(_points[2].x, _points[2].y, _points[2].z);
  _manual->position(_points[3].x, _points[3].y, _points[3].z);
  _manual->position(_points[4].x, _points[4].y, _points[4].z);
  _manual->position(_points[1].x, _points[1].y, _points[1].z);
  _manual->end();
}

void SceneGraphProximitySensor::__cap()
{
  _manual->begin("cap", Ogre::RenderOperation::OT_TRIANGLE_FAN);
  _manual->position(_points[0].x, _points[0].y, _points[0].z);
  _manual->position(_points[1].x, _points[1].y, _points[1].z);
  _manual->position(_points[2].x, _points[2].y, _points[2].z);
  _manual->position(_points[3].x, _points[3].y, _points[3].z);
  _manual->position(_points[4].x, _points[4].y, _points[4].z);
  _manual->setVisible(true);
  _manual->setCastShadows(false);
  _manual->end();
}

void SceneGraphProximitySensor::__body()
{
  _manual->begin("cap", Ogre::RenderOperation::OT_TRIANGLE_FAN);
  _manual->position(0.0, 0.0, 0.0);
  _manual->position(_points[1].x, _points[1].y, _points[1].z);
  _manual->position(_points[2].x, _points[2].y, _points[2].z);
  _manual->position(_points[3].x, _points[3].y, _points[3].z);
  _manual->position(_points[4].x, _points[4].y, _points[4].z);
  _manual->position(_points[1].x, _points[1].y, _points[1].z);
  _manual->setCastShadows(false);
  _manual->end();
}

void SceneGraphProximitySensor::__rays()
{
  _manual->begin("rays", Ogre::RenderOperation::OT_LINE_LIST);
  for(int i = 0; i < 5; i++)
  {
    _manual->position(0.0,          0.0,          0.0);
    _manual->position(_points[i].x, _points[i].y, _points[i].z);
  }
  _manual->setVisible(true);
  _manual->setCastShadows(false);
  _manual->end();
}
