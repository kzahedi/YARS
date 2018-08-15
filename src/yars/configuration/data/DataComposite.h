#ifndef __DATA_COMPOSITE_H__
#define __DATA_COMPOSITE_H__

#include "DataObjects.h"
#include "DataPhysicsParameter.h"

#include <vector>

using namespace std;

# define YARS_STRING_OBJECT_COMPOSITE            (char*)"composite"
# define YARS_STRING_OBJECT_COMPOSITE_DEFINITION (char*)"object_composite_definition"

/**
 * @brief Contains all information about a composite object
 *
 * <b> NOT IMPLEMENTED YET </b>
 */
class DataComposite : public DataObject
{
  public:
    DataComposite(DataNode *parent);

    void add(DataParseElement *element);

    DataObjects::iterator g_begin();
    DataObjects::iterator g_end();
    int g_size();
    DataObject* geom(int index);
    DataObject* body();

    DataComposite* _copy();
    void add(DataObject* object);

    void setBody(DataObject* body);
    void applyOffset(Pose pose); // overwriting DataObject::applyPose

    static void createXsd(XsdSpecification *spec);

    void _resetTo(const DataObject* other);

  private:
    DataObjects           _geoms;
};

#endif // __DATA_COMPOSITE_H__
