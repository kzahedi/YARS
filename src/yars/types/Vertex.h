#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <yars/defines/types.h>
#include <ostream>
#include <vector>

class Vertex 
{
  public:
    yReal x;
    yReal y;
    yReal z;
    yReal nx;
    yReal ny;
    yReal nz;
    yReal s;
    yReal t;
    bool  stGiven;

    Vertex();

    Vertex & operator=(const Vertex &v);
    bool     operator==(const Vertex &v);
    Vertex(const Vertex &v);

    friend std::ostream& operator<<(std::ostream& str, const Vertex& v)
    {
      if(v.stGiven)
      {
        str << "Position: " << v.x << ", " << v.y << ", " << v.z << ", Normals: " << v.nx << ", " << v.ny << ", " << v.nz << " UV: " << v.s << ", " << v.t;;
      }
      else
      {
        str << "Position: " << v.x << ", " << v.y << ", " << v.z << ", Normals: " << v.nx << ", " << v.ny << ", " << v.nz;
      }
      return str;
    };

};

typedef std::vector<Vertex> Vertices;

#endif //__VERTEX_H__
