#ifndef __DATA_COMMENT_H__
#define __DATA_COMMENT_H__

#include "DataNode.h"

# define COMMENT                                      "comment"
# define YARS_STRING_COMMENT                           (char*)COMMENT

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
