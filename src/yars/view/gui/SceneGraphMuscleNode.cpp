#include "SceneGraphMuscleNode.h"

#include <yars/defines/defaults.h>
#include <yars/configuration/data/DataCylinder.h>
#include <yars/configuration/data/DataCapsule.h>
#include <yars/configuration/data/DataBox.h>
#include <yars/configuration/data/DataSphere.h>

#include <OGRE/Ogre.h>
#include <OgreEdgeListBuilder.h>

#define SET_INDICES                                     \
  for (unsigned int i = 0; i < indices.size() - 2; i++) \
  {                                                     \
    if (i % 2 == 0)                                     \
    {                                                   \
      _manual->index(indices[i + 0]);                   \
      _manual->index(indices[i + 1]);                   \
      _manual->index(indices[i + 2]);                   \
    }                                                   \
    else                                                \
    {                                                   \
      _manual->index(indices[(i - 1) + 1]);             \
      _manual->index(indices[(i - 1) + 3]);             \
      _manual->index(indices[(i - 1) + 2]);             \
    }                                                   \
  }

SceneGraphMuscleNode::SceneGraphMuscleNode(DataMuscleActuator *data, Ogre::SceneNode *r, Ogre::SceneManager *sm)
    : SceneGraphObjectNode(r, sm)
{
  _data = data;
  // _dimension = _data->dimension();

  _radiusSrc = __getRadius(0);
  _radiusDst = __getRadius(1);

  _node = _root->createChildSceneNode();
  _manual = _sceneManager->createManualObject(_data->name());

  __body();

  _manual->setMaterialName(0, _data->texture(0));

  Ogre::EdgeData::EdgeGroupList::iterator itShadow, itEndShadow;
  for (itShadow = _manual->getEdgeList()->edgeGroups.begin(), itEndShadow = _manual->getEdgeList()->edgeGroups.end(); itShadow != itEndShadow; itShadow++)
    const_cast<Ogre::VertexData *>((*itShadow).vertexData)->prepareForShadowVolume();

  _node->attachObject(_manual);

  update();
}

void SceneGraphMuscleNode::update()
{

  _manual->clear();

  __body();

  _manual->setMaterialName(0, _data->texture(0));

  Ogre::EdgeData::EdgeGroupList::iterator itShadow, itEndShadow;
  for (itShadow = _manual->getEdgeList()->edgeGroups.begin(), itEndShadow = _manual->getEdgeList()->edgeGroups.end(); itShadow != itEndShadow; itShadow++)
    const_cast<Ogre::VertexData *>((*itShadow).vertexData)->prepareForShadowVolume();

  P3D p = _data->getMusclePosition();
  ::Quaternion q = _data->getMuscleQuaternion();
  ::Quaternion r(P3D(M_PI_2, 0.0, M_PI_2));
  q *= r;
  _node->setPosition(Ogre::Vector3(p.x, p.y, p.z));
  _node->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z));
}

////////////////////////////////////////////////////////////////////////////////
// Body
////////////////////////////////////////////////////////////////////////////////

void SceneGraphMuscleNode::__body()
{
  double x, y;
  double delta = 2.0 * M_PI / (double)DEFAULT_CAPPED_CYLINDER_QUALITY;
  double z = _data->getMuscleLength() / 2.0;

  double du = 1.0 / (double)DEFAULT_CAPPED_CYLINDER_QUALITY;
  double u = 0.0;

  _manual->setDynamic(true);
  _manual->begin("body", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
  std::vector<int> indices;
  int index = 0;
  for (int i = 0; i < DEFAULT_CAPPED_CYLINDER_QUALITY + 1; i++)
  {
    x = sin(delta * i);
    y = cos(delta * i);

    _manual->position(x * _radiusSrc, y * _radiusSrc, -z);
    _manual->normal(x, y, 0);
    _manual->textureCoord(u, 1);
    indices.push_back(index++);

    _manual->position(x * _radiusDst, y * _radiusDst, z);
    _manual->normal(x, y, 0);
    _manual->textureCoord(u, 0);
    indices.push_back(index++);

    u += du;
  }

  SET_INDICES;

  _manual->setVisible(true);
  _manual->setCastShadows(true);
  _manual->end();
}

double SceneGraphMuscleNode::__getRadius(int mode)
{
  double r = 0.0;
  DataObject *obj;
  if (mode == 0)
  {
    obj = _data->destinationAnchor();
  }
  else
  {
    obj = _data->sourceAnchor();
  }

  switch (obj->type())
  {
  case DATA_OBJECT_BOX:
    r = ((DataBox *)obj)->dimension().height;
    break;
  case DATA_OBJECT_SPHERE:
    r = ((DataSphere *)obj)->radius();
    break;
  case DATA_OBJECT_CYLINDER:
    r = ((DataCylinder *)obj)->dimension().radius;
    break;
  case DATA_OBJECT_CAPPED_CYLINDER:
    r = ((DataCapsule *)obj)->dimension().radius;
    break;
  }

  return r;
}