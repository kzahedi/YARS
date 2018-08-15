#ifndef __DATA_MACROS_H__
#define __DATA_MACROS_H__

#include "DataNode.h"
#include "DataMacro.h"

# define YARS_STRING_MACROS            (char*)"macros"
# define YARS_STRING_MACROS_DEFINITION (char*)"macros_definition"

#include <vector>
#include <string>

using namespace std;

class DataMacros : public std::vector<DataMacro*>, public DataNode
{
  public:
    DataMacros(DataNode *parent);
    virtual ~DataMacros();

    /**
     * @brief Returns the n-th macro
     *
     * @param index
     *
     * @return macro element
     * @sa DataMacro
     */
    DataMacro* macro(int index);

    /**
     * @brief Returns the macro with name given as parameter, or NULL otherwise.
     *
     * @param name
     *
     * @return macro
     * @sa DataMacro
     */
    DataMacro* macro(string name);

    /**
     * @brief Adds a DataParseElement to this node.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataMacros* copy();

    static void createXsd(XsdSpecification *spec);
};


#endif // __DATA_MACROS_H__



