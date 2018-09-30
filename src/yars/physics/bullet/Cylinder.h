#ifndef __CYLINDER_H__
#define __CYLINDER_H__

#include <yars/physics/bullet/Object.h>
#include <yars/configuration/data/DataCylinder.h>

class Cylinder : public Object
{
public:
  Cylinder(DataCylinder *cylinder);
  void init();

private:
  DataCylinder *_data;
};

#endif // __CYLINDER_H__
