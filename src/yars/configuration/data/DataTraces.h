#ifndef __DATA_TRACES_H__
#define __DATA_TRACES_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/configuration/data/DataTraceLine.h>
#include <yars/configuration/data/DataTracePoint.h>

#include <yars/types/Pose.h>

#include <vector>
#include <string>

# define TRACES                        "traces"
# define YARS_STRING_TRACES            (char*)TRACES
# define YARS_STRING_TRACES_DEFINITION (char*)TRACES DIVIDER DEFINITION

using namespace std;

class DataTraces : public DataNode
{
  public:
    /**
     * @brief Constructor
     *
     * @param parent
     */
    DataTraces(DataNode *parent);

    /**
     * @brief Destructor.
     */
    virtual ~DataTraces();

    /**
     * @brief Returns the name.
     *
     * @return name
     */
    string name();

    std::vector<DataTraceLine*>::iterator  l_begin();
    std::vector<DataTraceLine*>::iterator  l_end();
    int                               l_size();
    std::vector<DataTracePoint*>::iterator p_begin();
    std::vector<DataTracePoint*>::iterator p_end();
    int                               p_size();

    void add(DataParseElement *element);

    static void createXsd(XsdSpecification *spec);

    DataTraces* copy();

    void resetTo(DataTraces* other);

  private:
    string                     _name;
    std::vector<DataTraceLine*>     _lines;
    std::vector<DataTracePoint*>    _points;
};

#endif // __DATA_TRACES_H__

