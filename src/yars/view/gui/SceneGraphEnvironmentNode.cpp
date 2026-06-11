#include "SceneGraphEnvironmentNode.h"
#include "MaterialManager.h"

#include <yars/view/gui/SceneGraphObjectFactory.h>
#include <yars/view/gui/SceneGraphLightSourceNode.h>
#include <yars/util/macros.h>
#include <yars/configuration/YarsConfiguration.h>

SceneGraphEnvironmentNode::SceneGraphEnvironmentNode(
    DataEnvironment *data, Ogre::SceneNode *root, Ogre::SceneManager *sm)
    : SceneGraphObjectNode(root, sm)
{
  _data = data;
  _node = _root->createChildSceneNode();
  _x = 0.0;
  _y = 0.0;

  if (_data->groundGiven())
  {
    Ogre::Plane plane(Ogre::Vector3::UNIT_Z, 0);
    // 50×50 ground mesh (was 500×500). The smaller size balances visible
    // horizon coverage against texture tiling; the fixed shadow frustum in
    // OgreHandler's YarsFixedShadowCameraSetup is sized to cover it. 50m is
    // large enough to hide the horizon in typical YARS camera angles. Texture
    // tile count scales down proportionally (40→4) to keep visual density
    // similar.
    Ogre::MeshManager::getSingleton().createPlane("ground",
                                                  Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
                                                  50, 50, 25, 25, true, 1, 4, 4, Ogre::Vector3::UNIT_Y);
    _entity = _sceneManager->createEntity("GroundEntity", "ground");
    _node->attachObject(_entity);
    _entity->setCastShadows(false);

    // Use MaterialManager to resolve material names properly
    std::string materialName = data->texture();
    materialName = MaterialManager::instance()->resolveMaterialName(materialName);

    Ogre::MaterialPtr m = Ogre::MaterialManager::getSingleton().getByName(materialName);
    if (m.isNull())
    {
      // Don't create entity if material is not available to avoid crashes
      return;
    }

    // Check if material has techniques before accessing (some materials have RTSS-only)
    if (m->getNumTechniques() > 0)
    {
      Ogre::Technique *technique = m->getTechnique(0);
      if (technique && technique->getNumPasses() > 0)
      {
        Ogre::Pass *pass = technique->getPass(0);
        if (pass && pass->getNumTextureUnitStates() > 0)
        {
          _groundTextureUnitState = pass->getTextureUnitState(0);
        }
        else
        {
          _groundTextureUnitState = nullptr;
        }
      }
      else
      {
        _groundTextureUnitState = nullptr;
      }
    }
    else
    {
      _groundTextureUnitState = nullptr;
    }
    _entity->setMaterial(m);
  }

  int index = 0;
  for (auto m = _data->m_begin(); m != _data->m_end(); ++m)
  {
    Ogre::SceneNode *meshNode = _node->createChildSceneNode();
    _meshNodes.push_back(meshNode);

    stringstream oss;
    oss << _data->name() << index;
    Ogre::Entity *entity = sm->createEntity(oss.str(), (*m)->name());
    _entities.push_back(entity);

    if ((*m)->texture().size() > 0)
      entity->setMaterialName((*m)->texture());

    meshNode->setScale(Ogre::Vector3((*m)->scale().x, (*m)->scale().y, (*m)->scale().z));
    meshNode->attachObject(entity);
    P3D position = (*m)->pose().position;
    ::Quaternion q((*m)->pose().orientation);
    meshNode->setPosition(Ogre::Vector3(position.x, position.y, position.z));
    meshNode->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z));
    index++;
  }

  for (auto g = _data->g_begin(); g != _data->g_end(); ++g)
  {
    SceneGraphObjectNode *objectNode = SceneGraphObjectFactory::create(*g, root, sm);
    if (objectNode != nullptr)
      _objects.emplace_back(objectNode);
  }

  for (auto l = _data->l_begin(); l != _data->l_end(); ++l)
  {
    SceneGraphLightSourceNode *lightNode = new SceneGraphLightSourceNode(*l, root, sm);
    if (lightNode != nullptr)
      _lightSources.emplace_back(lightNode);
  }
}

void SceneGraphEnvironmentNode::update()
{
  for (auto &o : _objects) if (o) o->update();
  P3D p;
  __YARS_GET_CAMERA_POSITION(&p);
  // _node->setPosition(Ogre::Vector3(p.x, p.y, 0.0));
  // cout << p << endl;
  _dx = p.x - _x;
  _dy = p.y - _y;
  _x = p.x;
  _y = p.y;

  _sx = _dx / 50.0;
  _sy = _dy / 50.0;

  // cout << p.x << " " << p.y << endl;

  // Only animate texture if ground texture unit state exists
  // if (_groundTextureUnitState != nullptr) {
  //   _groundTextureUnitState->setScrollAnimation(-(float)p.x, -(float)p.y);
  //   _groundTextureUnitState->setScrollAnimation(10.0, -10.0);
  //   _groundTextureUnitState->setScrollAnimation( -p.x, -p.y);
  // }
}
