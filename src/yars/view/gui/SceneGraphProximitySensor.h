#ifndef __SCENE_GRAPH_PROXIMITY_SENSOR_H__
#define __SCENE_GRAPH_PROXIMITY_SENSOR_H__

#include <yars/view/gui/SceneGraphObjectNode.h>
#include <yars/configuration/data/DataGenericProximitySensor.h>
#include <yars/types/P3D.h>

#include <Ogre.h>

#include <vector>

using namespace std;

class SceneGraphProximitySensor : public SceneGraphObjectNode
{
public:
  SceneGraphProximitySensor(DataGenericProximitySensor *sensor, Ogre::SceneNode *, Ogre::SceneManager *);
  ~SceneGraphProximitySensor();

  void update();
  void reset(){};

private:
  void __cap();
  void __body();
  void __rays();

  void __setRay(int index, double length);
  void __updateContainers();
  void __updateRayColour(int index, bool collided);

  DataGenericProximitySensor *_data;
  Ogre::ManualObject *_manual;

  std::vector<::Quaternion> _quaternions;
  std::vector<::P3D> _points;
};

#endif // __SCENE_GRAPH_PROXIMITY_SENSOR_H__
