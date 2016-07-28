#ifndef __DATA_SIGNALS_H__
#define __DATA_SIGNALS_H__

#include "DataSignal.h"
#include "DataNode.h"

#include <vector>
using namespace std;

# define SIGNALS_LIST                                 "signals"
# define YARS_STRING_SIGNALS_LIST                      (char*)SIGNALS_LIST
# define YARS_STRING_SIGNALS_LIST_DEFINITION           (char*)SIGNALS_LIST         DIVIDER         DEFINITION

class DataSignals : public DataNode, public std::vector<DataSignal*>
{
  public:
    DataSignals(DataNode *parent);
    virtual ~DataSignals() { };

    void add(DataParseElement *element);

    static void createXsd(XsdSpecification *spec);

    DataSignals* copy();

  private:
};

#endif // __DATA_SIGNALS_H__
