#ifndef __PLY_H__
#define __PLY_H__

#include <yars/physics/bullet/Object.h>

#include <yars/configuration/data/DataPly.h>
#include <yars/util/PlyLoader.h>

#include <btBulletDynamicsCommon.h>

class Ply : public Object
{
  public:
    Ply(DataPly*, bool isComposite);
    ~Ply();

    void postPhysicsUpdate();

  private:
    void __rigid();
#ifdef USE_SOFT_BODIES
    void __soft();
#endif // USE_SOFT_BODIES

    DataPly                    *_data;

    btVector3                  *_vertices;
    int                        *_indices;

    btTriangleIndexVertexArray *_indexVertexArrays;
    btTriangleMesh             *_trimesh;

    int _nVertices;
    int _nTriangles;
    PlyData _pd;

};

#endif // __PLY_H__
