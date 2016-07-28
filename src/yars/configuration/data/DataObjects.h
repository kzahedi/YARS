#ifndef __DATA_OBJECTS_H__
#define __DATA_OBJECTS_H__

#include "DataObject.h"

#include <vector>

using namespace std;

class DataObjects : public std::vector<DataObject*>
{
  public:
    DataObjects() { };

    void add(DataObject *o)
    {
      push_back(o);
    };

};

#endif // __DATA_OBJECTS_H__


