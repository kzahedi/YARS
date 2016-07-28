#ifndef __DATA_CONTROLLERS_H__
#define __DATA_CONTROLLERS_H__

#include "DataController.h"

#include <vector>

using namespace std;

class DataControllers : public std::vector<DataController*>
{
  public:
    DataControllers() { };

    void add(DataController* controller)
    {
      push_back(controller);
    };
};

#endif // __DATA_CONTROLLERS_H__

