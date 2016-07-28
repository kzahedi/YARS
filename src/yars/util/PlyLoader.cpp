#include <yars/util/PlyLoader.h>

#include <yars/util/StringTokeniser.h>
#include <yars/types/Vertex.h>
#include <yars/util/FileSystemOperations.h>

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

PlyLoader* PlyLoader::_me = NULL;

PlyLoader* PlyLoader::instance()
{
  if(_me == NULL) _me = new PlyLoader();
  return _me;
}

PlyLoader::PlyLoader()
{
}

PlyLoader::~PlyLoader()
{
}

PlyData PlyLoader::get(string filename)
{
  // for(map<string, PlyData>::iterator i = _data.begin(); i != _data.end(); i++)
  // {
    // if(i->first == filename) return i->second;
  // }
  if(_data.find(filename) == _data.end())
  {
    return __load(filename);
  }
  return _data[filename];
}

PlyData PlyLoader::__load(string filename)
{
  if(!FileSystemOperations::doesFileExist(filename))
  {
    YarsErrorHandler::push("Ply loader: '%s' not found.", filename.c_str());
    exit(-1);
  }
  int nrOfTokens   = 0;
  int nrOfFaces    = 0;
  int nrOfVertices = 0;
  bool header      = true;
  
  int facesRead    = 0;
  int verticesRead = 0;

  vector< vector<int> > faces;

  Vertices  vertices;
  Triangles triangles;

  ifstream input(filename.c_str());
  for( string line; getline( input, line ); )
  {
    if(header)
    {
      if(line.find("element vertex") != string::npos)
      {
        vector<string> st = StringTokeniser::tokenise(line, " ");
        nrOfVertices = atoi(st[2].c_str());
      }
      if(line.find("element face") != string::npos)
      {
        vector<string> st = StringTokeniser::tokenise(line, " ");
        nrOfFaces = atoi(st[2].c_str());
      }
      if(line.find("property float") != string::npos)
      {
        nrOfTokens++;
      }
      if(line.find("end_header") != string::npos)
      {
        header = false;
      }
    }
    else
    {
      if(verticesRead < nrOfVertices)
      {
        vector<string> st = StringTokeniser::tokenise(line, " ");
        vector<float> values;
        for(vector<string>::iterator s = st.begin(); s != st.end(); s++)
        {
          values.push_back(atof((*s).c_str()));
        }
        verticesRead++;
        Vertex vertex;
        if(values.size() > 6)
        {
          vertex.x       = values[0];
          vertex.y       = values[1];
          vertex.z       = values[2];
          vertex.nx      = values[3];
          vertex.ny      = values[4];
          vertex.nz      = values[5];
          vertex.s       = values[6];
          vertex.t       = values[7];
          vertex.stGiven = true;
        }
        else
        {
          vertex.x       = values[0];
          vertex.y       = values[1];
          vertex.z       = values[2];
          vertex.nx      = values[3];
          vertex.ny      = values[4];
          vertex.nz      = values[5];
          vertex.stGiven = false;
        }
        vertices.push_back(vertex);
      }
      else if(facesRead < nrOfFaces)
      {
        vector<string> st = StringTokeniser::tokenise(line, " ");
        vector<int> values;
        for(vector<string>::iterator s = st.begin(); s != st.end(); s++)
        {
          values.push_back(atoi((*s).c_str()));
        }
        facesRead++;
        faces.push_back(values);
      }
    }
  }

  for(vector< vector<int> >::iterator v = faces.begin(); v != faces.end(); v++)
  {
    if((*v).size() == 4) // Triangle
    {
      Vertex a = vertices[(*v)[1]];
      Vertex b = vertices[(*v)[2]];
      Vertex c = vertices[(*v)[3]];

      P3D a_normal(a.nx, a.ny, a.nz);
      P3D b_normal(b.nx, b.ny, b.nz);
      P3D c_normal(c.nx, c.ny, c.nz);

      Triangle t((*v)[1], (*v)[2], (*v)[3],
          a_normal, b_normal, c_normal,
          P3D(a.x, a.y, a.z),
          P3D(b.x, b.y, b.z),
          P3D(c.x, c.y, c.z)
          );
      triangles.push_back(t);
    }
    else
    {
      Vertex a = vertices[(*v)[1]];
      Vertex b = vertices[(*v)[2]];
      Vertex c = vertices[(*v)[3]];
      Vertex d = vertices[(*v)[4]];

      P3D a_normal(a.nx, a.ny, a.nz);
      P3D b_normal(b.nx, b.ny, b.nz);
      P3D c_normal(c.nx, c.ny, c.nz);
      P3D d_normal(d.nx, d.ny, d.nz);

      if(a.stGiven)
      {
        Triangle t1((*v)[1], (*v)[3], (*v)[4],
            a_normal, c_normal, d_normal,
            P3D(a.x, a.y, a.z),
            P3D(c.x, c.y, c.z),
            P3D(d.x, d.y, d.z),
            a.s, a.t,
            c.s, c.t,
            d.s, d.t);
        Triangle t2((*v)[1], (*v)[2], (*v)[3],
            a_normal, b_normal, c_normal,
            P3D(a.x, a.y, a.z),
            P3D(b.x, b.y, b.z),
            P3D(c.x, c.y, c.z),
            a.s, a.t,
            b.s, b.t,
            c.s, c.t);
        triangles.push_back(t1);
        triangles.push_back(t2);
      }
      else
      {
        Triangle t1((*v)[1], (*v)[3], (*v)[4],
            a_normal, c_normal, d_normal,
            P3D(a.x, a.y, a.z),
            P3D(c.x, c.y, c.z),
            P3D(d.x, d.y, d.z)
            );
        Triangle t2((*v)[1], (*v)[2], (*v)[3],
            a_normal, b_normal, c_normal,
            P3D(a.x, a.y, a.z),
            P3D(b.x, b.y, b.z),
            P3D(c.x, c.y, c.z)
            );
        triangles.push_back(t1);
        triangles.push_back(t2);
      }
    }
  }

  PlyData pd(vertices, triangles);
  // pd.removeDoubles();
  _data.insert(std::make_pair(filename, pd));

  return pd;
}
