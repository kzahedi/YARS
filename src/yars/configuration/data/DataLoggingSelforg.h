#ifndef __DATA_LOGGING_SELFORG_H__
#define __DATA_LOGGING_SELFORG_H__

#include "DataNode.h"
#include "DataLoggingModule.h"

#include <string>
#include <vector>

using namespace std;

#define YARS_STRING_SELFORG_LOGGER            (char*)"selforg"
#define YARS_STRING_SELFORG_LOGGER_DEFINITION (char*)"selforg" DIVIDER "logging" DIVIDER DEFINITION

class DataLoggingSelforg : public DataLoggingModule, public DataNode
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataLoggingSelforg(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataLoggingSelforg();

    /**
     * @brief Adds a DataParseElement to this node.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataLoggingSelforg* copy();

    static void createXsd(XsdSpecification *spec);

    string filename();
    string name();

    bool useFile();
    bool useMatrixviz();
    bool useGuilogger();

    int  glStepSize();
    int  mvStepSize();

  private:
    string _name;
    string _filename;
    bool   _useFile;
    bool   _useMatrixviz;
    bool   _useGuilogger;
    int    _guiloggerStepSize;
    int    _matrixvizStepSize;
};

#endif // __DATA_LOGGING_SELFORG_H__



