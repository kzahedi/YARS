#ifndef __BOX_H__
#define __BOX_H__

#include <yars/physics/bullet/Object.h>

#include <yars/configuration/data/DataBox.h>

class Box : public Object
{
public:
  Box(DataBox *box);
  void init();

private:
  DataBox *_data;
};

#endif // __BOX_H__
