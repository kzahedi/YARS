#ifndef __CAPSULE_H__
#define __CAPSULE_H__

#include <yars/physics/bullet/Object.h>

#include <yars/configuration/data/DataCapsule.h>

class Capsule : public Object
{
public:
  Capsule(DataCapsule *capsule);

  void init();

private:
  DataCapsule *_data;
};

#endif // __CAPSULE_H__
