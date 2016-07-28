#ifndef __DATA_MACRO_INSTANCE_H__
#define __DATA_MACRO_INSTANCE_H__

#include "DataNode.h"
#include "DataMacros.h"
#include "DataObjects.h"

#include <string>
#include <vector>

using namespace std;

/**
 * @brief Contains all information about a macro
 *
 * <b> NOT IMPLEMENTED YET </b>
 */
class DataMacroInstance : public DataNode, public DataObjects
{
  public:
    DataMacroInstance(DataNode *parent, DataMacros *macros);
    virtual ~DataMacroInstance() { };

    void add(DataParseElement *element);

    DataMacroInstance* copy(DataNode *parent);

    string texture(int index);
    int nrOfTextures();

    static void createXsd(XsdSpecification *spec);

  private:
    void __setTexture(DataObject *o);

    DataMacros                  *_macros;
    Pose                         _pose;
    string                       _target;
    string                       _prefix;
    vector<string>               _textures;
};

#endif // __DATA_MACRO_INSTANCE_H__
