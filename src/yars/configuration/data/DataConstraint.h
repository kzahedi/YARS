#ifndef __DATA_CONSTRAINT_FACTORY_H__
#define __DATA_CONSTRAINT_FACTORY_H__

#include "DataNode.h"

class DataConstraint : public DataNode
{
  public:
    DataConstraint(DataNode *parent)
      : DataNode(parent)
    { }

    int type()
    {
      return _type;
    }

  protected:
    int _type;
};

#endif // __DATA_CONSTRAINT_FACTORY_H__


