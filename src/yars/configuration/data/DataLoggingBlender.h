#ifndef __DATA_LOGGING_BLENDER_H__
#define __DATA_LOGGING_BLENDER_H__

#include "DataNode.h"
#include "DataLoggingModule.h"

#include <string>
#include <vector>

using namespace std;

#define YARS_STRING_BLENDER_LOGGER            (char*)"blender"
#define YARS_STRING_BLENDER_LOGGER_DEFINITION (char*)"blender_logging_definition"

class DataLoggingBlender : public DataLoggingModule, public DataNode
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataLoggingBlender(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataLoggingBlender();

    /**
     * @brief Adds a DataParseElement to this node.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataLoggingBlender* copy();

    static void createXsd(XsdSpecification *spec);

    string filename();
    string cfg();
    int    framerate();

  private:
    string _filename;
    string _cfg;
    int    _framerate;
};

#endif // __DATA_LOGGING_BLENDER_H__



