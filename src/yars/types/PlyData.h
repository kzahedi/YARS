#ifndef __PLY_DATA_H__
#define __PLY_DATA_H__

#include "Vertex.h"
#include "Triangle.h"

namespace yars {

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

} // namespace yars

// Temporary global using directive for backward compatibility during namespace transition
using namespace yars;

#endif // __PLY_DATA_H__
