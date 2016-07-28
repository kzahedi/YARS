#ifndef __DATA_LOGGING_CSV_H__
#define __DATA_LOGGING_CSV_H__

#include "DataNode.h"
#include "DataLoggingModule.h"

#include <string>
#include <vector>

using namespace std;

#define YARS_STRING_CSV_LOGGER            (char*)"csv"
#define YARS_STRING_CSV_LOGGER_DEFINITION (char*)"csv" DIVIDER "logging" DIVIDER DEFINITION

class DataLoggingCSV : public DataLoggingModule, public DataNode
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataLoggingCSV(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataLoggingCSV();

    /**
     * @brief Adds a DataParseElement to this node.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataLoggingCSV* copy();

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
    bool   _debug;
};

#endif // __DATA_LOGGING_CSV_H__



