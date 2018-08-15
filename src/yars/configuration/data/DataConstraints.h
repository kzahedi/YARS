#ifndef __DATA_CONSTRAINTS_H__
#define __DATA_CONSTRAINTS_H__

#include "DataConstraint.h"
#include "DataNode.h"

#include <vector>
using namespace std;

# define YARS_STRING_CONSTRAINTS            (char*)"constraints"
# define YARS_STRING_CONSTRAINTS_DEFINITION (char*)"constraints_definition"

class DataConstraints : public DataNode
{
  public:
    DataConstraints(DataNode *parent);

    void add(DataParseElement *element);

    std::vector<DataConstraint*>::iterator c_begin();
    std::vector<DataConstraint*>::iterator c_end();
    int c_size();

    static void createXsd(XsdSpecification *spec);

  private:
    std::vector<DataConstraint*> _constraints;

};

#endif // __DATA_CONSTRAINTS_H__

