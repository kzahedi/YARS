#include "SceneGraphLightSourceNode.h"

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

SceneGraphLightSourceNode::SceneGraphLightSourceNode(
    DataPointLightSource *data, Ogre::SceneNode* r, Ogre::SceneManager* sm)
  : SceneGraphObjectNode(r,sm)
{
  _data = data;
  _node   = _root->createChildSceneNode();
  _manual = _sceneManager->createManualObject(_data->name());

  __topCap();
  __bottomCap();
  // for(unsigned int i = 0; i < _manual->getNumSections(); i++) _manual->setMaterialName(i, _data->texture());

  // Ogre::EdgeData::EdgeGroupList::iterator itShadow, itEndShadow;
  // for( itShadow=_manual->getEdgeList()->edgeGroups.begin(), itEndShadow=_manual->getEdgeList()->edgeGroups.end(); itShadow!=itEndShadow; itShadow++ )
    // const_cast<Ogre::VertexData*>((*itShadow).vertexData)->prepareForShadowVolume();

  _node->attachObject(_manual);

  update();
}

void SceneGraphLightSourceNode::update()
{
  P3D          p = _data->position();
  _node->setPosition(Ogre::Vector3(p.x, p.y, p.z));
}

void SceneGraphLightSourceNode::__topCap()
{
  yReal z     = 0;
  yReal delta = 2.0 * M_PI / (yReal)DEFAULT_CAPPED_CYLINDER_QUALITY;
  yReal r     = _data->size();
  for(int i = 1; i <= DEFAULT_CAPPED_CYLINDER_QUALITY / 2; i++)
  {
    stringstream oss;
    int index = 0;
    std::vector<int> indices;
    oss << "top " << i;
    _manual->begin(oss.str().c_str(), Ogre::RenderOperation::OT_TRIANGLE_STRIP);
    for(int j = 0; j <= DEFAULT_CAPPED_CYLINDER_QUALITY; j++)
    {
      yReal sj = sin(delta * j);
      yReal cj = cos(delta * j);
      yReal c1 = cos(delta * 0.5 * (i-1));
      yReal c2 = cos(delta * 0.5 * i);

      yReal x1 = sj * c1;
      yReal y1 = cj * c1;
      yReal z1 = sin(delta * 0.5 * (i-1));
      yReal u1 = sj * c1 * 0.5 + 0.5;
      yReal v1 = cj * c1 * 0.5 + 0.5;

      yReal x2 = sj * c2;
      yReal y2 = cj * c2;
      yReal z2 = sin(delta * 0.5 * i);
      yReal u2 = sj * c2 * 0.5 + 0.5;
      yReal v2 = cj * c2 * 0.5 + 0.5;

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

void SceneGraphLightSourceNode::__bottomCap()
{
  yReal z     = 0;
  yReal delta = 2.0 * M_PI / (yReal)DEFAULT_CAPPED_CYLINDER_QUALITY;
  yReal r     = _data->size();
  for(int i = 1; i <= DEFAULT_CAPPED_CYLINDER_QUALITY / 2; i++)
  {
    stringstream oss;
    int index = 0;
    std::vector<int> indices;
    oss << "bottom " << i;
    _manual->begin(oss.str().c_str(), Ogre::RenderOperation::OT_TRIANGLE_STRIP);
    for(int j = 0; j <= DEFAULT_CAPPED_CYLINDER_QUALITY; j++)
    {
      yReal sj = sin(delta * j);
      yReal cj = cos(delta * j);
      yReal c1 = cos(delta * 0.5 * (i-1));
      yReal c2 = cos(delta * 0.5 * i);

      yReal x1 = sj * c1;
      yReal y1 = cj * c1;
      yReal z1 = - sin(delta * 0.5 * (i-1));
      yReal u1 = sj * c1 * 0.5 + 0.5;
      yReal v1 = cj * c1 * 0.5 + 0.5;

      yReal x2 = sj * c2;
      yReal y2 = cj * c2;
      yReal z2 = - sin(delta * 0.5 * i);
      yReal u2 = sj * c2 * 0.5 + 0.5;
      yReal v2 = cj * c2 * 0.5 + 0.5;

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
