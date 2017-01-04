#include "SceneGraphObjectFactory.h"
#include <yars/view/gui/SceneGraphCapsuleNode.h>
#include <yars/view/gui/SceneGraphBoxNode.h>
#include <yars/view/gui/SceneGraphSphereNode.h>
#include <yars/view/gui/SceneGraphCylinderNode.h>
#include <yars/view/gui/SceneGraphSoftPlyNode.h>
#include <yars/view/gui/SceneGraphMeshNode.h>
#include <yars/view/gui/SceneGraphPlyNode.h>

#include <yars/util/YarsErrorHandler.h>


SceneGraphObjectNode* SceneGraphObjectFactory::create(
    DataObject* object, Ogre::SceneNode* r, Ogre::SceneManager* sm)
{
  if(object->visualise() == false) return NULL;
  if(object->m_size() > 0)         return __createMesh(object, r, sm);
  else
  {
    switch(object->type())
    {
      case DATA_OBJECT_BOX:
        return __createBox((DataBox*)object, r, sm);
        break;
      case DATA_OBJECT_SPHERE:
        return __createSphere((DataSphere*)object, r, sm);
        break;
      case DATA_OBJECT_CAPPED_CYLINDER:
        return __createCapsule((DataCapsule*)object, r, sm);
        break;
      case DATA_OBJECT_CYLINDER:
        return __createCylinder((DataCylinder*)object, r, sm);
        break;
      case DATA_OBJECT_PLY:
        if(((DataPly*)object)->isSoft()) return __createSoftPly((DataPly*)object, r, sm);
        return __createPly((DataPly*)object, r, sm);
        break;
      default:
        YarsErrorHandler::push("SceneGraphObjectNode::create. Unknown object type %d", object->type());
    }
    return NULL;
  }
}

SceneGraphObjectNode* SceneGraphObjectFactory::__createBox(
    DataBox *box, Ogre::SceneNode* node, Ogre::SceneManager* sm)
{
  return new SceneGraphBoxNode(box, node, sm);
}

SceneGraphObjectNode* SceneGraphObjectFactory::__createSphere(
    DataSphere *sphere, Ogre::SceneNode* node, Ogre::SceneManager* sm)
{
  return new SceneGraphSphereNode(sphere, node, sm);
}

SceneGraphObjectNode* SceneGraphObjectFactory::__createCapsule(
    DataCapsule *cappedCylinder, Ogre::SceneNode* node, Ogre::SceneManager* sm)
{
  return new SceneGraphCapsuleNode(cappedCylinder, node, sm);
}

SceneGraphObjectNode* SceneGraphObjectFactory::__createCylinder(
    DataCylinder *cylinder, Ogre::SceneNode* node, Ogre::SceneManager* sm)
{
  return new SceneGraphCylinderNode(cylinder, node, sm);
}

SceneGraphObjectNode* SceneGraphObjectFactory::__createSoftPly(
    DataPly *ply, Ogre::SceneNode* node, Ogre::SceneManager* sm)
{
  return new SceneGraphSoftPlyNode(ply, node, sm);
}


SceneGraphObjectNode* SceneGraphObjectFactory::__createMesh(
    DataObject *object, Ogre::SceneNode* node, Ogre::SceneManager* sm)
{
  return new SceneGraphMeshNode(object, node, sm);
}

SceneGraphObjectNode* SceneGraphObjectFactory::__createPly(
    DataPly *ply, Ogre::SceneNode* node, Ogre::SceneManager* sm)
{
  return new SceneGraphPlyNode(ply, node, sm);
}
