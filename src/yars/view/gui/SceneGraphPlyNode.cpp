#include "SceneGraphPlyNode.h"
#include "MaterialManager.h"

#include <yars/util/PlyLoader.h>

#include <OGRE/Ogre.h>
#include <OgreEdgeListBuilder.h>

SceneGraphPlyNode::SceneGraphPlyNode(DataPly *ply, Ogre::SceneNode* r, Ogre::SceneManager* sm)
  : SceneGraphObjectNode(r, sm)
{
  _data     = ply;
  _node     = _root->createChildSceneNode();

  int index = 0;
  for(vector<DataMeshVisualisation*>::iterator m = _data->m_begin(); m != _data->m_end(); m++)
  {
    Ogre::SceneNode* meshNode = _node->createChildSceneNode();
    _meshes.push_back(meshNode);

    stringstream oss;
    oss << _data->name() << index;
    Ogre::Entity *entity = sm->createEntity(oss.str(), (*m)->name());
    _entities.push_back(entity);

    if((*m)->texture().size() > 0) entity->setMaterialName(MaterialManager::instance()->resolveMaterialName((*m)->texture()));

    meshNode->setScale(Ogre::Vector3((*m)->scale().x, (*m)->scale().y, (*m)->scale().z));
    meshNode->attachObject(entity);
    entity->setCastShadows(true);
    P3D position = (*m)->pose().position;
    ::Quaternion q((*m)->pose().orientation);
    meshNode->setPosition(Ogre::Vector3(position.x, position.y, position.z));
    meshNode->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z));
    index++;
  }

  if(_data->m_size() == 0)
  {
    // Use temporary ManualObject, then convert to Mesh for shadow support
    _manual = _sceneManager->createManualObject(_data->name() + "_temp");

    PlyData pd = PlyLoader::instance()->get(_data->filename());

    stringstream oss;
    oss << _data->name() << " mesh";
    _manual->begin(oss.str(), Ogre::RenderOperation::OT_TRIANGLE_LIST);
    for(Vertices::iterator v = pd.vertices.begin(); v != pd.vertices.end(); v++)
    {
      _manual->position((*v).x, (*v).y, (*v).z);
      _manual->normal((*v).nx, (*v).ny, (*v).nz);
      if((*v).stGiven) _manual->textureCoord((*v).s, (*v).t);
    }
    for(Triangles::iterator t = pd.triangles.begin(); t != pd.triangles.end(); t++)
    {
      _manual->index((*t).a);
      _manual->index((*t).b);
      _manual->index((*t).c);
    }
    _manual->end();

    // Set material before conversion
    _manual->setMaterialName(0, MaterialManager::instance()->resolveMaterialName(_data->texture()));

    // Convert ManualObject to Mesh for proper shadow support
    std::string meshName = _data->name() + "_ply_mesh";
    Ogre::MeshPtr mesh = _manual->convertToMesh(meshName);
    mesh->buildEdgeList();

    // Create Entity from the mesh
    _plyEntity = _sceneManager->createEntity(_data->name(), mesh);
    _plyEntity->setCastShadows(true);
    _node->attachObject(_plyEntity);

    // Clean up temporary ManualObject
    _sceneManager->destroyManualObject(_manual);
    _manual = nullptr;
  }
  update();
}

void SceneGraphPlyNode::update()
{
  Pose pose      = _data->pose();
  ::Quaternion q = pose.q;
  P3D          p = pose.position;
  _node->setPosition(Ogre::Vector3(p.x, p.y, p.z));
  _node->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z));
}

