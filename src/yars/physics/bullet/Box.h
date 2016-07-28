#ifndef __BOX_H__
#define __BOX_H__

#include <yars/physics/bullet/Object.h>

#include <yars/configuration/data/DataBox.h>

class Box : public Object
{
  public:
    Box(DataBox *box, bool isComposite = false);

};

#endif // __BOX_H__
