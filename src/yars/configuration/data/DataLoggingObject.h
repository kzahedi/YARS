#ifndef __DATA_LOGGING_OBJECT_H__
#define __DATA_LOGGING_OBJECT_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/configuration/data/DataObject.h>

#include <yars/util/stl_macros.h>

#include <vector>
#include <string>

#define YARS_STRING_LOGGING_OBJECT            (char*)"object"
#define YARS_STRING_LOGGING_OBJECT_DEFINITION (char*)"object_logging_definition"

class DataLoggingObject : public DataNode, public std::vector<string>
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataLoggingObject(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataLoggingObject();

    /**
     * @brief Adds a DataParseElement to this node.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataLoggingObject* copy();

    static void createXsd(XsdSpecification *spec);

    string target();

    DataObject* object();
    void set(DataObject* object);
    int precision();

  private:
    string      _target;
    DataObject* _object;
    int         _precision;
};

#endif // __DATA_LOGGING_OBJECT_H__

