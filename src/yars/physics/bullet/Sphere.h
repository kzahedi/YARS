#ifndef __SPHERE_H__
#define __SPHERE_H__

#include <yars/physics/bullet/Object.h>

#include <yars/configuration/data/DataSphere.h>

class Sphere : public Object
{
public:
  Sphere(DataSphere *sphere);

  void init();

private:
  DataSphere *_data;
};

#endif // __SPHERE_H__
