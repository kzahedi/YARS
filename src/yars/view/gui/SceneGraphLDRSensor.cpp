#include "SceneGraphLDRSensor.h"

#include "types/Colour.h"

#include <OGRE/Ogre.h>

# define RESOLUTION 16
# define LENGTH     0.025

SceneGraphLDRSensor::SceneGraphLDRSensor(DataGenericLightDependentResistorSensor *data,
    Ogre::SceneNode* r, Ogre::SceneManager* sm)
: SceneGraphObjectNode(r,sm)
{
  // 5 Rays
  _data    = data;
  P3D position = _data->pose().position;
  _points.clear();

  _node    = _root->createChildSceneNode();
  _manual  = _sceneManager->createManualObject(_data->name());
  _node->setPosition(Ogre::Vector3(position.x, position.y, position.z));

  ::Quaternion q(_data->pose().orientation);
  _node->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z));

  yReal oa = _data->opening();

  _points.push_back(P3D(0.0, 0.0, 0.0));
  yReal radius = tan(oa) * LENGTH;
  // for(int i = RESOLUTION+2; i >= 0; i--)
  for(int i = 0; i < RESOLUTION+2; i++)
  {
    yReal x = sin((float)i*2.0*M_PI/(float)RESOLUTION) * radius;
    yReal y = cos((float)i*2.0*M_PI/(float)RESOLUTION) * radius;
    // _quaternions.push_back(::Quaternion(P3D(-oa,   oa, 0.0)));
    // _quaternions.push_back(::Quaternion(P3D( oa,   oa, 0.0)));
    // _quaternions.push_back(::Quaternion(P3D( oa,  -oa, 0.0)));
    _points.push_back(P3D(x, y, LENGTH));
  }

  __cap();
  __body();

  _manual->setMaterialName(0, "YARS/LDRSensor");
  _manual->setMaterialName(1, "YARS/LDRSensor");

  _node->attachObject(_manual);
}

SceneGraphLDRSensor::~SceneGraphLDRSensor()
{
}

void SceneGraphLDRSensor::update()
{
}

void SceneGraphLDRSensor::__cap()
{
  _manual->begin("cap", Ogre::RenderOperation::OT_TRIANGLE_FAN);

  for(int i = (int)_points.size()-1; i > 1; i--)
  // for(int i = 1; i < (int)_points.size(); i++)
  {
    _manual->position(_points[i].x, _points[i].y, _points[i].z);
  }
  _manual->setVisible(true);
  _manual->setCastShadows(false);
  _manual->end();
}

void SceneGraphLDRSensor::__body()
{
  _manual->begin("body", Ogre::RenderOperation::OT_TRIANGLE_FAN);
  for(std::vector< ::P3D>::iterator i = _points.begin(); i != _points.end(); i++)
  {
    _manual->position((*i).x, (*i).y, (*i).z);
  }
  _manual->setVisible(true);
  _manual->setCastShadows(false);
  _manual->end();
}


