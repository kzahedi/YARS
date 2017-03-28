#ifndef __DATA_ACTUATORS_H__
#define __DATA_ACTUATORS_H__

#include "DataActuator.h"

#include <vector>

class DataActuators : public std::vector<DataActuator*>
{
  public:
    DataActuators() { };

    void add(DataActuator* actuator)
    {
      push_back(actuator);
    };
};

#endif // __DATA_ACTUATORS_H__
