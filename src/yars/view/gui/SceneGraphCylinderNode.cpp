#include "SceneGraphCylinderNode.h"

#include <yars/defines/defaults.h>

#define SET_INDICES\
  for(unsigned int i = 0; i < indices.size()-2; i++)\
  {\
    if(i%2==0)\
    {\
      _manual->index(indices[i+0]);\
      _manual->index(indices[i+1]);\
      _manual->index(indices[i+2]);\
    }\
    else\
    {\
      _manual->index(indices[(i-1)+1]);\
      _manual->index(indices[(i-1)+3]);\
      _manual->index(indices[(i-1)+2]);\
    }\
  }

#define SET_INDICES_2\
  for(unsigned int i = 0; i < indices.size()-2; i++)\
  {\
      _manual->index(indices[i+0]);\
      _manual->index(indices[i+1]);\
      _manual->index(indices[i+2]);\
  }

SceneGraphCylinderNode::SceneGraphCylinderNode(
    DataCylinder *data, Ogre::SceneNode* r, Ogre::SceneManager* sm)
  : SceneGraphObjectNode(r,sm)
{
  _data = data;
  _dimension = _data->dimension();

  _node   = _root->createChildSceneNode();
  _manual = _sceneManager->createManualObject(_data->name());

  __topCap();
  __bottomCap();
  __body();

  _manual->setMaterialName(0, _data->texture(0));
  _manual->setMaterialName(1, _data->texture(1));
  _manual->setMaterialName(2, _data->texture(2));

  Ogre::EdgeData::EdgeGroupList::iterator itShadow, itEndShadow;
  for( itShadow=_manual->getEdgeList()->edgeGroups.begin(), itEndShadow=_manual->getEdgeList()->edgeGroups.end(); itShadow!=itEndShadow; itShadow++ )
    const_cast<Ogre::VertexData*>((*itShadow).vertexData)->prepareForShadowVolume();

  _node->attachObject(_manual);

  update();
}

void SceneGraphCylinderNode::update()
{
  Pose pose = _data->pose();
  ::Quaternion q = pose.q;
  P3D          p = pose.position;
  _node->setPosition(Ogre::Vector3(p.x, p.y, p.z));
  _node->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z));
}

void SceneGraphCylinderNode::__topCap()
{
  double z     = _dimension.height / 2.0;
  double delta = M_PI / (double)DEFAULT_CAPPED_CYLINDER_QUALITY;
  double r     = _dimension.radius;

  int index = 0;
  std::vector<int> indices;
  _manual->begin("top", Ogre::RenderOperation::OT_TRIANGLE_FAN);
  _manual->position(0,0,z);
  _manual->normal(0,0,1);
  _manual->textureCoord(0.5,0.5);
  indices.push_back(index++);
  for(int i = 0; i < 2*DEFAULT_CAPPED_CYLINDER_QUALITY + 1; i++)
  {
    double x = sin(delta * i) * r;
    double y = cos(delta * i) * r;
    double u = sin(delta * i) * 0.5 + 0.5;
    double v = cos(delta * i) * 0.5 + 0.5;

    _manual->position(x,y,z);
    _manual->normal(0,0,1);
    _manual->textureCoord(u,v);
    indices.push_back(index++);
  }

  for(unsigned int i = 0; i < indices.size()-1; i++)
  {
    _manual->index(0);
    _manual->index(indices[i+1]);
    _manual->index(indices[i]);
  }

  // _manual->index(0);
  // _manual->index(indices[indices.size()]);
  // _manual->index(indices[1]);

  _manual->setVisible(true);
  _manual->setCastShadows(true);
  _manual->end();
}

void SceneGraphCylinderNode::__bottomCap()
{

  double z     = -_dimension.height / 2.0;
  double delta = M_PI / (double)DEFAULT_CAPPED_CYLINDER_QUALITY;
  double r     = _dimension.radius;

  int index = 0;
  std::vector<int> indices;
  _manual->begin("bottom", Ogre::RenderOperation::OT_TRIANGLE_FAN);
  _manual->position(0,0,z);
  _manual->normal(0,0,-1);
  _manual->textureCoord(0.5,0.5);
  indices.push_back(index++);
  for(int i = 0; i < 2*DEFAULT_CAPPED_CYLINDER_QUALITY + 1; i++)
  {
    double x = sin(delta * i) * r;
    double y = cos(delta * i) * r;
    double u = sin(delta * i) * 0.5 + 0.5;
    double v = cos(delta * i) * 0.5 + 0.5;

    _manual->position(x,y,z);
    _manual->normal(0,0,-1);
    _manual->textureCoord(u,v);
    indices.push_back(index++);
  }

  for(unsigned int i = 0; i < indices.size()-1; i++)
  {
    _manual->index(0);
    _manual->index(indices[i]);
    _manual->index(indices[i+1]);
  }

  // _manual->index(0);
  // _manual->index(indices[1]);
  // _manual->index(indices[indices.size()]);

  _manual->setVisible(true);
  _manual->setCastShadows(true);
  _manual->end();
}

void SceneGraphCylinderNode::__body()
{
  double x, y;
  double z     = _dimension.height / 2.0;
  double delta = 2.0 * M_PI / (double)DEFAULT_CAPPED_CYLINDER_QUALITY;
  double r     = _dimension.radius;
  double du    = 1.0/(double)DEFAULT_CAPPED_CYLINDER_QUALITY;
  double u     = 0.0;

  _manual->begin("body", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
  std::vector<int> indices;
  int index = 0;
  for(int i = 0; i < DEFAULT_CAPPED_CYLINDER_QUALITY + 1; i++)
  {
    x = sin(delta * i);
    y = cos(delta * i);

    _manual->position(x*r,y*r,-z);
    _manual->normal(x,y,0);
    _manual->textureCoord(u,1);
    indices.push_back(index++);

    _manual->position(x*r,y*r,z);
    _manual->normal(x,y,0);
    _manual->textureCoord(u,0);
    indices.push_back(index++);

    u += du;
  }

  SET_INDICES;

  _manual->setVisible(true);
  _manual->setCastShadows(true);
  _manual->end();
}

