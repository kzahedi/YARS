#ifndef __DATA_MESH_VISUALISATION_H__
#define __DATA_MESH_VISUALISATION_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/types/Pose.h>
#include <yars/types/P3D.h>

# define MESH_VISUALISATION                              (char*)"mesh"
# define YARS_STRING_OBJECT_MESH_VISUALISATION           MESH_VISUALISATION
# define YARS_STRING_OBJECT_MESH_VISUALISATION_DEFINTION MESH_VISUALISATION DIVIDER DEFINITION

class DataMeshVisualisation : public DataNode
{
  public:
    DataMeshVisualisation(DataNode *parent);
    ~DataMeshVisualisation();

    void add(DataParseElement *element);

    static void createXsd(XsdSpecification *spec);

    P3D    scale();
    string name();
    bool   use();
    Pose   pose();
    DataMeshVisualisation* copy();
    string texture();

  private:
    P3D    _scale;
    string _name;
    bool   _use;
    Pose   _pose;
    string _texture;
};

#endif // __DATA_MESH_VISUALISATION_H__
