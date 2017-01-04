#ifndef __SCENE_GRAPH_OBJECT_FACTORY_H__
#define __SCENE_GRAPH_OBJECT_FACTORY_H__

#include <yars/view/gui/SceneGraphObjectNode.h>
#include <yars/configuration/data/DataObject.h>
#include <yars/configuration/data/DataBox.h>
#include <yars/configuration/data/DataSphere.h>
#include <yars/configuration/data/DataCapsule.h>
#include <yars/configuration/data/DataCylinder.h>
#include <yars/configuration/data/DataPly.h>

class SceneGraphObjectFactory
{
  public:
    static SceneGraphObjectNode* create(DataObject*, Ogre::SceneNode*, Ogre::SceneManager*);

  private:
    static SceneGraphObjectNode* __createBox(DataBox*,           Ogre::SceneNode*, Ogre::SceneManager*);
    static SceneGraphObjectNode* __createSphere(DataSphere*,     Ogre::SceneNode*, Ogre::SceneManager*);
    static SceneGraphObjectNode* __createCapsule(DataCapsule*,   Ogre::SceneNode*, Ogre::SceneManager*);
    static SceneGraphObjectNode* __createCylinder(DataCylinder*, Ogre::SceneNode*, Ogre::SceneManager*);
    static SceneGraphObjectNode* __createSoftPly(DataPly*,       Ogre::SceneNode*, Ogre::SceneManager*);
    static SceneGraphObjectNode* __createMesh(DataObject*,       Ogre::SceneNode*, Ogre::SceneManager*);
    static SceneGraphObjectNode* __createPly(DataPly*,           Ogre::SceneNode*, Ogre::SceneManager*);
};

#endif // __SCENE_GRAPH_OBJECT_FACTORY_H__


