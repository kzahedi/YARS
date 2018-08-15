#ifndef __DATA_FILTER_H__
#define __DATA_FILTER_H__

#include "DataNode.h"
#include "DataParameter.h"

# define YARS_STRING_FILTER            (char*)"filter"
# define YARS_STRING_FILTER_DEFINITION (char*)"filter_definition"

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
