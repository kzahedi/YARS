#ifndef __BULLET_OBJECT_FACTORY_H__
#define __BULLET_OBJECT_FACTORY_H__

#include <yars/configuration/data/DataObject.h>
#include <yars/configuration/data/DataBox.h>
#include <yars/configuration/data/DataSphere.h>
#include <yars/configuration/data/DataCylinder.h>
#include <yars/configuration/data/DataCapsule.h>
#include <yars/configuration/data/DataComposite.h>
#include <yars/configuration/data/DataPly.h>
#include <yars/physics/bullet/Object.h>

#include <string>

using namespace std;

class ObjectFactory
{
  public:

    static Object* create(DataObject *object);

  private:

    static Object* __sphere(DataObject   *object);
    static Object* __capsule(DataObject  *object);
    static Object* __cylinder(DataObject *object);
    static Object* __compound(DataObject *object);
    static Object* __box(DataObject      *object);
    static Object* __ply(DataObject      *object);
};

#endif // __BULLET_OBJECT_FACTORY_H__

