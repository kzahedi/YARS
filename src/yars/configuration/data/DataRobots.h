#ifndef __DATA_ROBOTS_H__
#define __DATA_ROBOTS_H__

#include "DataNode.h"
#include "DataRobot.h"
#include "DataMacros.h"

# define ROBOTS                               "robots"
# define YARS_STRING_ROBOTS            (char*)ROBOTS
# define YARS_STRING_ROBOTS_DEFINITION (char*)ROBOTS DIVIDER DEFINITION

#include <vector>

using namespace std;

/**
 * @brief Contains all robots.
 */
class DataRobots : public DataNode, public std::vector<DataRobot*>
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataRobots(DataNode *parent);

    virtual ~DataRobots();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataRobot* robot(int index);

    void setMacros(DataMacros *macros);

    static void createXsd(XsdSpecification *spec);

    DataRobots* copy();

    void resetTo(const DataRobots *robots);

  private:
    DataMacros *_macros;

};

#endif // __DATA_MOVEABLES_H__
