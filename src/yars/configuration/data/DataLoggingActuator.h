#ifndef __DATA_LOGGING_ACTUATOR_H__
#define __DATA_LOGGING_ACTUATOR_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/configuration/data/DataActuator.h>
#include <yars/configuration/data/DataActuators.h>

#include <yars/util/stl_macros.h>

#include <vector>
#include <string>

#define YARS_STRING_LOGGING_ACTUATOR            (char*)"actuator"
#define YARS_STRING_LOGGING_ACTUATOR_DEFINITION (char*)"actuator_logging_definition"

#define INTERNAL         6602
#define EXTERNAL         6603
#define DESIRED          6604
#define APPLIED_FORCE    6605
#define APPLIED_VELOCITY 6606

class DataLoggingActuator : public DataNode, public std::vector<string>
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataLoggingActuator(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataLoggingActuator();

    /**
     * @brief Adds a DataParseElement to this node.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataLoggingActuator* copy();

    static void createXsd(XsdSpecification *spec);

    string target();

    DataActuator* actuator();
    void set(DataActuator* actuator);
    int precision();
    std::vector<int> order();

  private:
    string        _target;
    DataActuator* _actuator;
    int           _precision;
    std::vector<int> _order;

};

#endif // __DATA_LOGGING_ACTUATOR_H__


