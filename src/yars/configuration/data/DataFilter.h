#ifndef __DATA_FILTER_H__
#define __DATA_FILTER_H__

#include "DataNode.h"
#include "DataParameter.h"

# define FILTER                        "filter"
# define YARS_STRING_FILTER            (char*)FILTER
# define YARS_STRING_FILTER_DEFINITION (char*)FILTER DIVIDER DEFINITION

#include <vector>

using namespace std;

class DataFilter : public DataNode, public std::vector<DataParameter*>
{
  public:
    DataFilter(DataNode *parent);
    virtual ~DataFilter();

    void add(DataParseElement *element);

    string module();

    DataFilter* copy();

    DataParameter* parameter(int index);

    static void createXsd(XsdSpecification *spec);

  private:
    string                 _module;
};

#endif // __DATA_FILTER_H__
