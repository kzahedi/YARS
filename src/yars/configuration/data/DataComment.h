#ifndef __DATA_COMMENT_H__
#define __DATA_COMMENT_H__

#include "DataNode.h"

# define YARS_STRING_COMMENT (char*)"comment"

/**
 * @brief
 * @todo parse comment
 */
class DataComment : public DataNode
{
  public:
    DataComment(DataNode *parent);

};

#endif // __DATA_COMMENT_H__
