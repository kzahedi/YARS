#include "PlyData.h"

#include <vector>
#include <iostream>


using namespace std;

PlyData& PlyData::operator=(const PlyData &v)
{
  vertices  = v.vertices;
  triangles = v.triangles;
  return *this;
}

PlyData::PlyData() { }

PlyData::PlyData(const PlyData &v)
{
  vertices  = v.vertices;
  triangles = v.triangles;
}


PlyData::PlyData(Vertices v, Triangles t)
{
  vertices  = v;
  triangles = t;
}

void PlyData::removeDoubles()
{
  Vertices         new_vertices;
  std::vector<int> new_indices;
  new_indices.resize(vertices.size());
  for (int i = 0; i < new_indices.size(); i++) new_indices[i] = -1;

  for(int i = 0; i < vertices.size(); i++)
  {
    bool found = false;
    int index  = -1;
    for(int j = 0; j < new_vertices.size(); j++)
    {
      if(vertices[i] == new_vertices[j])
      {
        found = true;
        index = j;
        break;
      }
    }
    if(!found)
    {
      new_indices[i] = new_vertices.size();
      new_vertices.push_back(vertices[i]);
    }
    else
    {
      new_indices[i] = index;
    }
  }

  for (size_t i = 0; i < triangles.size(); i++)
  {
    if(new_indices[triangles[i].a] != -1) triangles[i].a =  new_indices[triangles[i].a];
    if(new_indices[triangles[i].b] != -1) triangles[i].b =  new_indices[triangles[i].b];
    if(new_indices[triangles[i].c] != -1) triangles[i].c =  new_indices[triangles[i].c];
  }

  vertices = new_vertices;
}
