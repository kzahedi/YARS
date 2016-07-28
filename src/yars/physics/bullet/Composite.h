#ifndef __COMPOUND_H__
#define __COMPOUND_H__

#include <yars/physics/bullet/Object.h>
#include <yars/configuration/data/DataComposite.h>

#include <btBulletDynamicsCommon.h>

// just required to delete btCompoundShape
class Composite : public Object
{
  public:
    Composite(DataComposite *data);

    void reset();
    void setInitialTransform(btTransform transform);

  private:
    btTransform _initialTransform;

};

#endif // __COMPOUND_H__


