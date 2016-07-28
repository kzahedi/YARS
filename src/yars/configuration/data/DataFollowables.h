#ifndef __DATA_FOLLOWABLES_H__
#define __DATA_FOLLOWABLES_H__

#include "DataNode.h"
#include "DataObjects.h"

#include <vector>
#include <string>

using namespace std;

# define YARS_STRING_FOLLOWABLES            (char*)"followables"
# define YARS_STRING_FOLLOWABLES_DEFINITION (char*)"followables" DIVIDER DEFINITION

class DataFollowables : public DataNode, public std::vector<string>
{
  public:
    DataFollowables(DataNode *parent);
    virtual ~DataFollowables();

   /**
     * @brief Adds a DataParseElement to this node.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    void add(DataObject* o);

    DataObjects::iterator o_begin();
    DataObjects::iterator o_end();
    int                           o_size();
    DataObject*                   followable(int index);

    DataFollowables* copy();
    static void createXsd(XsdSpecification *spec);

  private:

    DataObjects _objects;

};

#endif // __DATA_FOLLOWABLES_H__


