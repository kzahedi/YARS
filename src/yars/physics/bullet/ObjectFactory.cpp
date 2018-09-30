#include <yars/physics/bullet/ObjectFactory.h>

#include <yars/util/macros.h>

#include <yars/configuration/YarsConfiguration.h>
#include <yars/types/Quaternion.h>
#include <yars/util/YarsErrorHandler.h>

#include <yars/physics/bullet/Box.h>
#include <yars/physics/bullet/Sphere.h>
#include <yars/physics/bullet/Capsule.h>
#include <yars/physics/bullet/Composite.h>
#include <yars/physics/bullet/MyMotionState.h>
#include <yars/physics/bullet/MyCompositeMotionState.h>
#include <yars/physics/bullet/Cylinder.h>
#include <yars/physics/bullet/Ply.h>
#include <yars/physics/bullet/World.h>

#include <iostream>
#include <btBulletDynamicsCommon.h>

using namespace std;

Object *ObjectFactory::create(DataObject *object)
{
  switch (object->type())
  {
  case DATA_OBJECT_SPHERE:
    return __sphere(object);
    break;
  case DATA_OBJECT_BOX:
    return __box(object);
    break;
  case DATA_OBJECT_CYLINDER:
    return __cylinder(object);
    break;
  case DATA_OBJECT_CAPPED_CYLINDER:
    return __capsule(object);
    break;
  case DATA_OBJECT_COMPOSITE:
    return __compound(object);
    break;
  case DATA_OBJECT_PLY:
    return __ply(object);
    break;
  }
  YarsErrorHandler::push("ObjectFactory::create error. Unknown type given %d.", object->type());
  return NULL;
}

Object *ObjectFactory::__sphere(DataObject *object)
{
  return new Sphere((DataSphere *)object);
}

Object *ObjectFactory::__box(DataObject *object)
{
  return new Box((DataBox *)object);
}

Object *ObjectFactory::__ply(DataObject *object)
{
  return new Ply((DataPly *)object);
}

Object *ObjectFactory::__capsule(DataObject *object)
{
  return new Capsule((DataCapsule *)object);
}

Object *ObjectFactory::__cylinder(DataObject *object)
{
  return new Cylinder((DataCylinder *)object);
}

Object *ObjectFactory::__compound(DataObject *object)
{
  return new Composite((DataComposite *)object);
}
