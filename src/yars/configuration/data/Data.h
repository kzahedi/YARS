#ifndef __DATA_H__
#define __DATA_H__



#include "DataRobotSimulationDescription.h"

#include <vector>

using namespace std;

class Data
{
  public:
    static Data* instance();
    static void close();
    ~Data();
    DataRobotSimulationDescription* get(int index);
    DataRobotSimulationDescription* newSpecification();
    DataRobotSimulationDescription* current();
    DataRobotSimulationDescription* last();
    void initialise(int index = 0);
    XsdSpecification* xsd();
    void clear();
    int  size();

  private:
    Data();
    static Data                            *_me;
    DataRobotSimulationDescription         *_current;
    std::vector<DataRobotSimulationDescription*> _description;
};

#endif // __DATA_H__
