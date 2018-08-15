#ifndef __DATA_PLY_H__
#define __DATA_PLY_H__

#include "DataObject.h"
#include "pthread.h"

#include "DataSoftBodyParameters.h"



# define YARS_STRING_OBJECT_PLY                   (char*)"ply"
# define YARS_STRING_OBJECT_PLY_DEFINTION         (char*)"object_ply_definition"
# define YARS_STRING_FILE_NAME                    (char*)"filename"
# define YARS_STRING_PLY_VISUALISATION_DEFINITION (char*)"ply_visualisation_definition"

/**
 * @brief Implements a ply object.
 *
 * <b> NOT IMPLEMENTED YET </b>
 */
class DataPly : public DataObject
{
  public:
    DataPly(DataNode *parent);
    ~DataPly();

    void add(DataParseElement *element);
    string texture();
    string filename();
    bool   isConvex();
    bool   isSoft();

    void   lock();
    void   setVertex(int i, double x, double y, double z);
    void   setNormal(int i, double x, double y, double z);
    void   setIndex(int i, int index);
    void   unlock();

    void   setNumberOfVertices(int n);
    void   setNumberOfNormals(int n);
    void   setNumberOfIndices(int n);

    static void createXsd(XsdSpecification *spec);

    DataSoftBodyParameters* parameter();

    DataPly* _copy();
    void      _resetTo(const DataObject* other);

    vector<P3D>::const_iterator v_begin();
    vector<P3D>::const_iterator v_end();
    int                         v_size();
    P3D vertex(int index);

    vector<P3D>::const_iterator n_begin();
    vector<P3D>::const_iterator n_end();
    P3D normal(int index);

    vector<int>::const_iterator i_begin();
    vector<int>::const_iterator i_end();
    int index(int index);

  private:
    string                  _filename;
    string                  _texture;
    bool                    _isConvex;
    bool                    _isSoft;
    int                     _numberOfVertices;
    int                     _numberOfIndices;
    int                     _numberOfNormals;
    vector<P3D>             _vertices;
    vector<P3D>             _normals;
    vector<int>             _indices;
    pthread_mutex_t         _mutex;
    DataSoftBodyParameters *_parameters;
};

#endif // __DATA_PLY_H__
