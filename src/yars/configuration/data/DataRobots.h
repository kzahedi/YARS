#ifndef __DATA_ROBOTS_H__
#define __DATA_ROBOTS_H__

#include "DataNode.h"
#include "DataRobot.h"

# define YARS_STRING_ROBOTS            (char*)"robots"
# define YARS_STRING_ROBOTS_DEFINITION (char*)"robots_definition"

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

    DataRobots* copy();

    void resetTo(const DataRobots *robots);
};

#endif // __DATA_MOVEABLES_H__
