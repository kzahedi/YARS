#include "SceneGraphSoftPlyNode.h"

#include "util/PlyLoader.h"

#include "configuration/YarsConfiguration.h"


SceneGraphSoftPlyNode::SceneGraphSoftPlyNode(DataPly *ply, Ogre::SceneNode* r, Ogre::SceneManager* sm)
  : SceneGraphObjectNode(r, sm)
{
  _lastStep = 0;
  _data   = ply;
  _node   = _root->createChildSceneNode();
  _manual = _sceneManager->createManualObject(_data->name());
  PlyData pd = PlyLoader::instance()->get(_data->filename().c_str());
  _vertices = pd.vertices;

  stringstream oss;
  oss << _data->name() << " mesh";

  _manual->setDynamic(true);
  _manual->begin(oss.str(), Ogre::RenderOperation::OT_TRIANGLE_LIST);
  for(int i = 0; i < _data->v_size(); i++)
  {
    P3D p     = _data->vertex(i);
    P3D n     = _data->normal(i);
    int index = _data->index(i);
    _manual->position(p.x, p.y, p.z);
    _manual->normal(n.x, n.y, n.z);
    _manual->index(index);
    if(_vertices[0].stGiven) _manual->textureCoord(_vertices[i].s, _vertices[i].t);
  }

  _manual->end();
  _manual->setCastShadows(true);

  _node->attachObject(_manual);
  _manual->setMaterialName(0, _data->texture());
}

void SceneGraphSoftPlyNode::update()
{
  int index = 0;
  _manual->beginUpdate(0);
  _data->lock();
  for(int i = 0; i < _data->v_size(); i++)
  {
    P3D p     = _data->vertex(i);
    P3D n     = _data->normal(i);
    int index = _data->index(i);
    _manual->position(p.x, p.y, p.z);
    _manual->normal(n.x, n.y, n.z);
    _manual->index(index);
    if(_vertices[0].stGiven) _manual->textureCoord(_vertices[i].s, _vertices[i].t);
  }
  _data->unlock();
  _manual->end();

}
