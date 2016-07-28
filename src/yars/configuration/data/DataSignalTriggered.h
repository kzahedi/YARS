#ifndef __DATA_SIGNAL_TRIGGERED_H__
#define __DATA_SIGNAL_TRIGGERED_H__

#include "DataSignal.h"
#include "DataParameter.h"
#include <vector>
#include <string>

# define TRIGGERED                                       "triggered"
# define YARS_STRING_TRIGGERED_SIGNAL             (char*)TRIGGERED
# define YARS_STRING_TRIGGERED_SIGNAL_DEFINITION  (char*)TRIGGERED DIVIDER DEFINITION

using namespace std;

class DataSignalTriggered : public DataSignal, public std::vector<DataParameter*>
{
  public:
    DataSignalTriggered(DataNode *parent);

    void add(DataParseElement *element);
    string name();
    string module();
    string key();

    static void createXsd(XsdSpecification *spec);

    DataSignalTriggered* _copy();

  private:
    string                 _name;
    string                 _module;
    string                 _key;
};

#endif // __DATA_SIGNAL_TRIGGERED_H__
