#include "SceneGraphMeshNode.h"
#include "OgreHandler.h"
#include "PlanarShadowProjector.h"

SceneGraphMeshNode::SceneGraphMeshNode(DataObject *data, Ogre::SceneNode* r, Ogre::SceneManager* sm)
  : SceneGraphObjectNode(r, sm)
{
  _data     = data;
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

    entity->getMesh()->buildEdgeList();

    if((*m)->texture().size() > 0) entity->setMaterialName((*m)->texture());

    meshNode->setScale(Ogre::Vector3((*m)->scale().x, (*m)->scale().y, (*m)->scale().z));
    meshNode->attachObject(entity);
    Pose pose = (*m)->pose();
    P3D position = pose.position;
    ::Quaternion q(pose.orientation);
    meshNode->setPosition(Ogre::Vector3(position.x, position.y, position.z));
    meshNode->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z));

    // Register with the planar shadow projector so a translucent shadow
    // proxy is spawned and updated in lockstep with this mesh.
    if (auto *psp = yars::OgreHandler::instance()->getPlanarShadowProjector())
    {
      psp->registerCaster(meshNode, (*m)->name());
    }

    index++;
  }

  update();
}

void SceneGraphMeshNode::update()
{
  Pose pose      = _data->pose();
  ::Quaternion q = pose.q;
  P3D          p = pose.position;
  _node->setPosition(Ogre::Vector3(p.x, p.y, p.z));
  _node->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z));
}

