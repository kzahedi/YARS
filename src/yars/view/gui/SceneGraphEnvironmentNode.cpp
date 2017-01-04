#include "SceneGraphEnvironmentNode.h"

#include <yars/view/gui/SceneGraphObjectFactory.h>
#include <yars/view/gui/SceneGraphLightSourceNode.h>
#include <yars/util/macros.h>
#include <yars/configuration/YarsConfiguration.h>

SceneGraphEnvironmentNode::SceneGraphEnvironmentNode(
    DataEnvironment *data, Ogre::SceneNode* root, Ogre::SceneManager* sm)
 :SceneGraphObjectNode(root, sm)
{
  _data = data;
  _node = _root->createChildSceneNode();
  _x    = 0.0;
  _y    = 0.0;

  if(_data->groundGiven())
  {
    Ogre::Plane plane(Ogre::Vector3::UNIT_Z, 0 );
    Ogre::MeshManager::getSingleton().createPlane("ground",
        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
        500,500,50,50,true,1,40,40,Ogre::Vector3::UNIT_Y);
    _entity = _sceneManager->createEntity("GroundEntity", "ground");
    _node->attachObject(_entity);
    _entity->setCastShadows(false);
    Ogre::MaterialPtr m = Ogre::MaterialManager::getSingleton().getByName(data->texture());
    _groundTextureUnitState = m->getTechnique( 0 )->getPass( 0 )->getTextureUnitState( 0 );
    _entity->setMaterial(m);
  }

  int index = 0;
  for(vector<DataMeshVisualisation*>::iterator m = _data->m_begin(); m != _data->m_end(); m++)
  {
    Ogre::SceneNode* meshNode = _node->createChildSceneNode();
    _meshNodes.push_back(meshNode);

    stringstream oss;
    oss << _data->name() << index;
    Ogre::Entity *entity = sm->createEntity(oss.str(), (*m)->name());
    _entities.push_back(entity);

    if((*m)->texture().size() > 0) entity->setMaterialName((*m)->texture());

    meshNode->setScale(Ogre::Vector3((*m)->scale().x, (*m)->scale().y, (*m)->scale().z));
    meshNode->attachObject(entity);
    P3D position = (*m)->pose().position;
    ::Quaternion q((*m)->pose().orientation);
    meshNode->setPosition(Ogre::Vector3(position.x, position.y, position.z));
    meshNode->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z));
    index++;
  }

  for(DataObjects::iterator g = _data->g_begin(); g != _data->g_end(); g++)
  {
    SceneGraphObjectNode *objectNode = SceneGraphObjectFactory::create(*g, root, sm);
    if(objectNode != NULL) _objects.push_back(objectNode);
  }

  for(DataPointLightSources::iterator l = _data->l_begin(); l != _data->l_end(); l++)
  {
    SceneGraphLightSourceNode *lightNode = new SceneGraphLightSourceNode(*l, root, sm);
    if(lightNode != NULL) _lightSources.push_back(lightNode);
  }
}

SceneGraphEnvironmentNode::~SceneGraphEnvironmentNode()
{
  FOREACH(SceneGraphObjectNode*, o, _objects) delete *o;
  _objects.clear();
}


void SceneGraphEnvironmentNode::update()
{
  FOREACH(SceneGraphObjectNode*, o, _objects)  if(*o != NULL) (*o)->update();
  P3D p;
  __YARS_GET_CAMERA_POSITION(&p);
  // _node->setPosition(Ogre::Vector3(p.x, p.y, 0.0));
  // cout << p << endl;
  _dx = p.x - _x;
  _dy = p.y - _y;
  _x = p.x;
  _y = p.y;

  _sx = _dx / 500.0;
  _sy = _dy / 500.0;

  // cout << p.x << " " << p.y << endl;

  // _groundTextureUnitState->setScrollAnimation(-(float)p.x, -(float)p.y);
  // _groundTextureUnitState->setScrollAnimation(10.0, -10.0);
  // _groundTextureUnitState->setScrollAnimation( -p.x, -p.y);
}
