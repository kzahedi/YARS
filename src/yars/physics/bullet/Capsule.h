#ifndef __CAPSULE_H__
#define __CAPSULE_H__

#include <yars/physics/bullet/Object.h>

#include <yars/configuration/data/DataCapsule.h>

class Capsule : public Object
{
  public:
    Capsule(DataCapsule *capsule, bool isComposite = false);

};

#endif // __CAPSULE_H__
