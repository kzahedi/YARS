#ifndef __DATA_SIGNAL_PERIODIC_H__
#define __DATA_SIGNAL_PERIODIC_H__

#include "DataSignal.h"
#include "DataParameter.h"
#include <vector>
#include <string>

# define PERIODIC                                       "periodic"
# define YARS_STRING_PERIODIC_SIGNAL             (char*)PERIODIC
# define YARS_STRING_PERIODIC_SIGNAL_DEFINITION  (char*)PERIODIC DIVIDER DEFINITION

using namespace std;

class DataSignalPeriodic : public DataSignal, public std::vector<DataParameter*>
{
  public:
    DataSignalPeriodic(DataNode *parent);

    void add(DataParseElement *element);
    string name();
    string module();

    static void createXsd(XsdSpecification *spec);

    DataSignalPeriodic* _copy();

  private:
    string                 _name;
    string                 _module;

};

#endif // __DATA_SIGNAL_PERIODIC_H__
