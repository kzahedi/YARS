#ifndef __DATA_LOGGING_CONTROLLER_H__
#define __DATA_LOGGING_CONTROLLER_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/configuration/data/DataController.h>
#include <yars/configuration/data/DataControllers.h>

#include <yars/util/stl_macros.h>

#include <vector>
#include <string>

#define YARS_STRING_LOGGING_CONTROLLER            (char*)"controller"
#define YARS_STRING_LOGGING_CONTROLLER_DEFINITION (char*)"controller_logging_definition"

#define INTERNAL 6602
#define EXTERNAL 6603

class DataLoggingController : public DataNode, public std::vector<string>
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataLoggingController(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataLoggingController();

    /**
     * @brief Adds a DataParseElement to this node.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataLoggingController* copy();

    static void createXsd(XsdSpecification *spec);

    string target();

    DataController* controller();
    void set(DataController* controller);
    int precision();
    std::vector<int> order();

  private:
    string          _target;
    DataController *_controller;
    int             _precision;
    std::vector<int>     _order;
};

#endif // __DATA_LOGGING_CONTROLLER_H__
