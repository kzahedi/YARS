#ifndef __DATA_SIGNAL_H__
#define __DATA_SIGNAL_H__

#include "DataNode.h"

class DataSignal : public DataNode
{
  public:

    DataSignal(DataNode *parent)
      : DataNode(parent)
    { }

    int type()
    {
      return _type;
    }

    DataSignal* copy()
    {
      DataSignal *copy = _copy();
      copy->_type = _type;
      return copy;
    }

    virtual DataSignal* _copy() = 0;

  protected:
    int _type;
};

#endif // __DATA_SIGNAL_H__
