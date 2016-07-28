#ifndef __PLY_DATA_H__
#define __PLY_DATA_H__

#include "Vertex.h"
#include "Triangle.h"

class PlyData
{
  public:
    Vertices  vertices;
    Triangles triangles;

    PlyData(Vertices, Triangles);
    PlyData & operator=(const PlyData &d);
    PlyData(const PlyData &d);
    PlyData();

    void removeDoubles();
};

#endif // __PLY_DATA_H__
