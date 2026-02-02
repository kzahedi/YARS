#include "SceneGraphBoxNode.h"
#include "MaterialManager.h"

#include <OGRE/Ogre.h>
#include <OgreEdgeListBuilder.h>

SceneGraphBoxNode::SceneGraphBoxNode(DataBox *box, Ogre::SceneNode *r, Ogre::SceneManager *sm)
    : SceneGraphObjectNode(r, sm)
{
  _data = box;
  _node = _root->createChildSceneNode();

  // Use a temporary ManualObject to build geometry, then convert to Mesh for shadow support
  _manual = _sceneManager->createManualObject(_data->name() + "_temp");

  WHDDimension d = _data->dimension();
  P3D dim;
  dim.x = d.width;
  dim.y = d.depth;
  dim.z = d.height;
  dim *= 0.5;

#define TRIANGLE             \
  _manual->index(0);         \
  _manual->index(1);         \
  _manual->index(2);         \
  _manual->index(1);         \
  _manual->index(3);         \
  _manual->index(2);         \
  _manual->setVisible(true);

  _manual->begin("face1", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
  _manual->position(+dim.x, +dim.y, +dim.z);
  _manual->textureCoord(0, 0);
  _manual->normal(0, 0, 1);
  _manual->position(-dim.x, +dim.y, +dim.z);
  _manual->textureCoord(1, 0);
  _manual->normal(0, 0, 1);
  _manual->position(+dim.x, -dim.y, +dim.z);
  _manual->textureCoord(0, 1);
  _manual->normal(0, 0, 1);
  _manual->position(-dim.x, -dim.y, +dim.z);
  _manual->textureCoord(1, 1);
  _manual->normal(0, 0, 1);
  TRIANGLE;
  _manual->end();

  _manual->begin("face2", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
  _manual->position(-dim.x, +dim.y, +dim.z);
  _manual->textureCoord(0, 0);
  _manual->normal(-1, 0, 0);
  _manual->position(-dim.x, +dim.y, -dim.z);
  _manual->textureCoord(1, 0);
  _manual->normal(-1, 0, 0);
  _manual->position(-dim.x, -dim.y, +dim.z);
  _manual->textureCoord(0, 1);
  _manual->normal(-1, 0, 0);
  _manual->position(-dim.x, -dim.y, -dim.z);
  _manual->textureCoord(1, 1);
  _manual->normal(-1, 0, 0);
  TRIANGLE;
  _manual->end();

  _manual->begin("face3", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
  _manual->position(-dim.x, -dim.y, -dim.z);
  _manual->textureCoord(0, 0);
  _manual->normal(0, 0, -1);
  _manual->position(-dim.x, +dim.y, -dim.z);
  _manual->textureCoord(1, 0);
  _manual->normal(0, 0, -1);
  _manual->position(+dim.x, -dim.y, -dim.z);
  _manual->textureCoord(0, 1);
  _manual->normal(0, 0, -1);
  _manual->position(+dim.x, +dim.y, -dim.z);
  _manual->textureCoord(1, 1);
  _manual->normal(0, 0, -1);
  TRIANGLE;
  _manual->end();

  _manual->begin("face4", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
  _manual->position(+dim.x, +dim.y, -dim.z);
  _manual->textureCoord(0, 0);
  _manual->normal(1, 0, 0);
  _manual->position(+dim.x, +dim.y, +dim.z);
  _manual->textureCoord(1, 0);
  _manual->normal(1, 0, 0);
  _manual->position(+dim.x, -dim.y, -dim.z);
  _manual->textureCoord(0, 1);
  _manual->normal(1, 0, 0);
  _manual->position(+dim.x, -dim.y, +dim.z);
  _manual->textureCoord(1, 1);
  _manual->normal(1, 0, 0);
  TRIANGLE;
  _manual->end();

  _manual->begin("face5", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
  _manual->position(+dim.x, +dim.y, +dim.z);
  _manual->textureCoord(0, 0);
  _manual->normal(0, 1, 0);
  _manual->position(+dim.x, +dim.y, -dim.z);
  _manual->textureCoord(1, 0);
  _manual->normal(0, 1, 0);
  _manual->position(-dim.x, +dim.y, +dim.z);
  _manual->textureCoord(0, 1);
  _manual->normal(0, 1, 0);
  _manual->position(-dim.x, +dim.y, -dim.z);
  _manual->textureCoord(1, 1);
  _manual->normal(0, 1, 0);
  TRIANGLE;
  _manual->end();

  _manual->begin("face6", Ogre::RenderOperation::OT_TRIANGLE_STRIP);
  _manual->position(-dim.x, -dim.y, +dim.z);
  _manual->textureCoord(0, 0);
  _manual->normal(0, -1, 0);
  _manual->position(-dim.x, -dim.y, -dim.z);
  _manual->textureCoord(1, 0);
  _manual->normal(0, -1, 0);
  _manual->position(+dim.x, -dim.y, +dim.z);
  _manual->textureCoord(0, 1);
  _manual->normal(0, -1, 0);
  _manual->position(+dim.x, -dim.y, -dim.z);
  _manual->textureCoord(1, 1);
  _manual->normal(0, -1, 0);
  TRIANGLE;
  _manual->end();

  // Set materials on ManualObject sections
  for (int i = 0; i < 6; i++)
  {
    std::string materialName = _data->texture(i);
    materialName = MaterialManager::instance()->resolveMaterialName(materialName);
    _manual->setMaterialName(i, materialName);
  }

  // Convert ManualObject to Mesh for proper shadow support
  std::string meshName = _data->name() + "_mesh";
  Ogre::MeshPtr mesh = _manual->convertToMesh(meshName);

  // Build edge list for stencil shadows
  mesh->buildEdgeList();

  // Create Entity from the mesh
  _entity = _sceneManager->createEntity(_data->name(), mesh);
  _entity->setCastShadows(true);

  // Attach entity to scene node
  _node->attachObject(_entity);

  // Clean up the temporary ManualObject
  _sceneManager->destroyManualObject(_manual);
  _manual = nullptr;

  update();
}

void SceneGraphBoxNode::update()
{
  Pose pose = _data->pose();
  ::Quaternion q = pose.q;
  P3D p = pose.position;
  _node->setPosition(Ogre::Vector3(p.x, p.y, p.z));
  _node->setOrientation(Ogre::Quaternion(q.w, q.x, q.y, q.z));
}
