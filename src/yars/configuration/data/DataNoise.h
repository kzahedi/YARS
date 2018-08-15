#ifndef __DATA_NOISE_H__
#define __DATA_NOISE_H__

#include "DataNode.h"
#include "DataParameter.h"

# define YARS_STRING_NOISE            (char*)"noise"
# define YARS_STRING_NOISE_DEFINITION (char*)"noise_definition"

#include <string>
#include <vector>

using namespace std;

class DataNoise : public DataNode, public std::vector<DataParameter*>
{
  public:
    DataNoise(DataNode *parent);
    virtual ~DataNoise();

    string module();
    void add(DataParseElement *element);

    DataParameter* parameter(int index);

    static void createXsd(XsdSpecification *spec);
    DataNoise* copy();

  private:
    string  _module;
};
#endif // __DATA_NOISE_H__
