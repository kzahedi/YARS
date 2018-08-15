#ifndef __DATA_LOGGING_CONSOLE_H__
#define __DATA_LOGGING_CONSOLE_H__

#include "DataNode.h"
#include "DataLoggingModule.h"

#include <string>
#include <vector>

using namespace std;

#define YARS_STRING_CONSOLE            (char*)"console"
#define YARS_STRING_CONSOLE_DEFINITION (char*)"console_logging_definition"

class DataLoggingConsole : public DataLoggingModule, public DataNode
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataLoggingConsole(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataLoggingConsole();

    /**
     * @brief Adds a DataParseElement to this node.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataLoggingConsole* copy();

    static void createXsd(XsdSpecification *spec);

  private:
};

#endif // __DATA_LOGGING_CONSOLE_H__


