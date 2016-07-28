#include "Vertex.h"

#include <math.h>


Vertex::Vertex()
{
  x       = 0.0;
  y       = 0.0;
  z       = 0.0;
  nx      = 0.0;
  ny      = 0.0;
  nz      = 0.0;
  s       = 0.0;
  t       = 0.0;
  stGiven = false;
}


Vertex& Vertex::operator=(const Vertex &v)
{
  x       = v.x;
  y       = v.y;
  z       = v.z;
  nx      = v.nx;
  ny      = v.ny;
  nz      = v.nz;
  s       = v.s;
  t       = v.t;
  stGiven = v.stGiven;
  return *this;
}

Vertex::Vertex(const Vertex &v)
{
  x       = v.x;
  y       = v.y;
  z       = v.z;
  nx      = v.nx;
  ny      = v.ny;
  nz      = v.nz;
  s       = v.s;
  t       = v.t;
  stGiven = v.stGiven;
}


bool Vertex::operator==(const Vertex &v)
{
  return
    (fabs(x  - v.x)  < 0.00000001) &&
    (fabs(y  - v.y)  < 0.00000001) &&
    (fabs(z  - v.z)  < 0.00000001); // &&
    // (fabs(nx - v.nx) < 0.00000001) &&
    // (fabs(ny - v.ny) < 0.00000001) &&
    // (fabs(nz - v.nz) < 0.00000001) &&
    // (fabs(s  - v.s)  < 0.00000001) &&
    // (fabs(t  - v.t)  < 0.00000001) &&
    // stGiven == v.stGiven;
}
