#include "SceneGraphCapsuleNode.h"

#include <yars/defines/defaults.h>

#include <sstream>

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

SceneGraphCapsuleNode::SceneGraphCapsuleNode(
    DataCapsule *data, Ogre::SceneNode* r, Ogre::SceneManager* sm)
  : SceneGraphObjectNode(r,sm)
{
  _data = data;
  _dimension = _data->dimension();

  _node   = _root->createChildSceneNode(_data->name());
  _manual = _sceneManager->createManualObject(_data->name());

  __body();
  __topCap();
  __bottomCap();

  _manual->setMaterialName(0, _data->texture(2));

  for(int i = 1; i <= DEFAULT_CAPPED_CYLINDER_QUALITY/2 + 1; i++) _manual->setMaterialName(i, _data->texture(0));
  for(int i = DEFAULT_CAPPED_CYLINDER_QUALITY/2 + 1; i < DEFAULT_CAPPED_CYLINDER_QUALITY+3; i++) _manual->setMaterialName(i, _data->texture(1));

  Ogre::EdgeData::EdgeGroupList::iterator itShadow, itEndShadow;
  for( itShadow=_manual->getEdgeList()->edgeGroups.begin(), itEndShadow=_manual->getEdgeList()->edgeGroups.end(); itShadow!=itEndShadow; itShadow++ )
    const_cast<Ogre::VertexData*>((*itShadow).vertexData)->prepareForShadowVolume();

  _node->attachObject(_manual);

  update();
}

void SceneGraphCapsuleNode::update()
{
  Pose pose = _data->pose();
  ::Quaternion q = pose.q;
  P3D          p = pose.position;
  _node->setPosition(Ogre::Vector3(p.x, p.y, p.z));
  _node->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z));
}

void SceneGraphCapsuleNode::__topCap()
{
  double z     = _dimension.height / 2.0;
  double delta = 2.0 * M_PI / (double)DEFAULT_CAPPED_CYLINDER_QUALITY;
  double r     = _dimension.radius;
  for(int i = 0; i <= DEFAULT_CAPPED_CYLINDER_QUALITY / 2; i++)
  {
    stringstream oss;
    int index = 0;
    vector<int> indices;
    oss << "top " << i;
    _manual->begin(oss.str().c_str(), Ogre::RenderOperation::OT_TRIANGLE_STRIP);
    for(int j = 0; j <= DEFAULT_CAPPED_CYLINDER_QUALITY; j++)
    {
      double sj = sin(delta * j);
      double cj = cos(delta * j);
      double c1 = cos(delta * 0.5 * (i-1));
      double c2 = cos(delta * 0.5 * i);

      double x1 = sj * c1;
      double y1 = cj * c1;
      double z1 = sin(delta * 0.5 * (i-1));
      double u1 = sj * c1 * 0.5 + 0.5;
      double v1 = cj * c1 * 0.5 + 0.5;

      double x2 = sj * c2;
      double y2 = cj * c2;
      double z2 = sin(delta * 0.5 * i);
      double u2 = sj * c2 * 0.5 + 0.5;
      double v2 = cj * c2 * 0.5 + 0.5;

      _manual->position(x1*r,y1*r,z + z1 * r);
      _manual->normal(x1,y1,z1);
      _manual->textureCoord(u1,v1);
      indices.push_back(index++);

      _manual->position(x2*r,y2*r,z + z2 * r);
      _manual->normal(x2,y2,z2);
      _manual->textureCoord(u2,v2);
      indices.push_back(index++);
    }

    SET_INDICES;
    _manual->setVisible(true);
    _manual->setCastShadows(true);
    _manual->end();
  }

   
     
}

void SceneGraphCapsuleNode::__bottomCap()
{
  double z     = - _dimension.height / 2.0;
  double delta = 2.0 * M_PI / (double)DEFAULT_CAPPED_CYLINDER_QUALITY;
  double r     = _dimension.radius;
  for(int i = 0; i <= DEFAULT_CAPPED_CYLINDER_QUALITY / 2; i++)
  {
    stringstream oss;
    int index = 0;
    std::vector<int> indices;
    oss << "bottom " << i;
    _manual->begin(oss.str().c_str(), Ogre::RenderOperation::OT_TRIANGLE_STRIP);
    for(int j = 0; j <= DEFAULT_CAPPED_CYLINDER_QUALITY; j++)
    {
      double sj = sin(delta * j);
      double cj = cos(delta * j);
      double c1 = cos(delta * 0.5 * (i-1));
      double c2 = cos(delta * 0.5 * i);

      double x1 = sj * c1;
      double y1 = cj * c1;
      double z1 = - sin(delta * 0.5 * (i-1));
      double u1 = sj * c1 * 0.5 + 0.5;
      double v1 = cj * c1 * 0.5 + 0.5;

      double x2 = sj * c2;
      double y2 = cj * c2;
      double z2 = - sin(delta * 0.5 * i);
      double u2 = sj * c2 * 0.5 + 0.5;
      double v2 = cj * c2 * 0.5 + 0.5;

      _manual->position(x2*r,y2*r,z + z2 * r);
      _manual->normal(x2,y2,z2);
      _manual->textureCoord(u2,v2);
      indices.push_back(index++);

      _manual->position(x1*r,y1*r,z + z1 * r);
      _manual->normal(x1,y1,z1);
      _manual->textureCoord(u1,v1);
      indices.push_back(index++);

    }
    SET_INDICES;
    _manual->setVisible(true);
    _manual->setCastShadows(true);
    _manual->end();
  }
}

void SceneGraphCapsuleNode::__body()
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
