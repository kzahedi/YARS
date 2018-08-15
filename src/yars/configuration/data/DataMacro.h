#ifndef __DATA_MACRO_H__
#define __DATA_MACRO_H__

#include <string>

#include "DataNode.h"
#include "DataObjects.h"

# define YARS_STRING_OBJECT_MACRO           (char*)"macro"
# define YARS_STRING_OBJECT_MACRO_DEFINTION (char*)"object_macro_definition"
# define YARS_STRING_MACRO                  (char*)"macro"
# define YARS_STRING_MACRO_DEFINITION       (char*)"macro_definition"

class DataMacro : public DataObjects, public DataNode
{
  public:
    DataMacro(DataNode *parent);
    virtual ~DataMacro();

    string name();

    DataMacro* copy();

    static void createXsd(XsdSpecification *spec);

    void add(DataParseElement *element);

  private:
    string _name;
};

#endif // __DATA_MACRO_H__

