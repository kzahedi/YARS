#ifndef __DATA_LOGGING_FILE_H__
#define __DATA_LOGGING_FILE_H__

#include "DataNode.h"
#include "DataLoggingModule.h"

#include <string>
#include <vector>

using namespace std;

#define YARS_STRING_FILE_LOGGER            (char*)"file"
#define YARS_STRING_FILE_LOGGER_DEFINITION (char*)"file" DIVIDER "logging" DIVIDER DEFINITION

class DataLoggingFile : public DataLoggingModule, public DataNode
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataLoggingFile(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataLoggingFile();

    /**
     * @brief Adds a DataParseElement to this node.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataLoggingFile* copy();

    static void createXsd(XsdSpecification *spec);

    string filename();
    bool   useDate();
    bool   useTimeStep();
    bool   isLogging(unsigned int timestep);
    bool   debug();

  private:
    string _filename;
    bool   _useDate;
    int    _start;
    int    _stop;
    bool   _useTimeStep;
    int    _debug;

};

#endif // __DATA_LOGGING_FILE_H__



