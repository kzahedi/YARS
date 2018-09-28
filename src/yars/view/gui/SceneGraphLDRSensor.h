#ifndef __SCENE_GRAPH_LDRY_SENSOR_H__
#define __SCENE_GRAPH_LDRY_SENSOR_H__

#include <yars/view/gui/SceneGraphObjectNode.h>
#include <yars/configuration/data/DataGenericLightDependentResistorSensor.h>
#include <yars/types/P3D.h>

#include <Ogre.h>

#include <vector>

using namespace std;

class SceneGraphLDRSensor : public SceneGraphObjectNode
{
public:
  SceneGraphLDRSensor(DataGenericLightDependentResistorSensor *sensor, Ogre::SceneNode *, Ogre::SceneManager *);
  ~SceneGraphLDRSensor();

  void update();
  void reset(){};

private:
  void __cap();
  void __body();

  DataGenericLightDependentResistorSensor *_data;
  Ogre::ManualObject *_manual;

  std::vector<::P3D> _points;
};

#endif // __SCENE_GRAPH_LDRY_SENSOR_H__
