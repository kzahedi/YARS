#ifndef __DATA_NODE_H__
#define __DATA_NODE_H__

#include "DataParseElement.h"
#include <yars/configuration/xsd/specification/specification.h>

#include <string>
#include <ostream>
#include <iostream>

# define DIVIDER                        "_"
# define DEFINITION                     "definition"

/**
 * @brief Basic class for all objects that store XML data.
 */
class DataNode
{
  public:

    /**
     * @brief Default constructor. Takes parent node as parameter
     *
     * @param parent
     */
    DataNode(DataNode *parent);

    /**
     * @brief Destructor, does nothing
     */
    virtual ~DataNode();

    /**
     * @brief This function must be implemented by every data storing class. It
     * takes a DataParseElement and should decide if the contained data is
     * either stored, passed on to a child, or if the current node is set to the
     * parent.
     *
     * @param DataParseElement
     * @sa DataParseElement
     */
    virtual void add(DataParseElement* ) = 0;

  protected:
    DataNode        *parent;
    static DataNode *current;
};

#endif // __DATA_NODE_H__
