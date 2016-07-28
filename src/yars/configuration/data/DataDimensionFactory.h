#ifndef __DATA_DIMENSION_FACTORY_H__
#define __DATA_DIMENSION_FACTORY_H__

#include <yars/types/WHDDimension.h>
#include <yars/types/RHDimension.h>

# define WIDTH                                        "width"
# define RADIUS                                       "radius"
# define HEIGHT                                       "height"
# define DEPTH                                        "depth"
# define WIDTH_HEIGHT                                 WIDTH DIVIDER HEIGHT
# define WIDTH_HEIGHT_DEPTH                           WIDTH DIVIDER HEIGHT DIVIDER DEPTH

# define YARS_STRING_DEPTH                         (char*)DEPTH
# define YARS_STRING_HEIGHT                        (char*)HEIGHT
# define YARS_STRING_RADIUS                        (char*)RADIUS
# define YARS_STRING_RADIUS_HEIGHT_DEFINITION      (char*)RADIUS             DIVIDER HEIGHT     DIVIDER DEFINITION
# define YARS_STRING_WIDTH                         (char*)WIDTH
# define YARS_STRING_WIDTH_HEIGHT_DEFINITION       (char*)WIDTH_HEIGHT       DIVIDER DEFINITION
# define YARS_STRING_WIDTH_HEIGHT_DEPTH_DEFINITION (char*)WIDTH_HEIGHT_DEPTH DIVIDER DEFINITION

class DataDimensionFactory
{
  public:
    static void set(WHDDimension &dimension, DataParseElement *element)
    {
      element->set(YARS_STRING_WIDTH,  dimension.width);
      element->set(YARS_STRING_HEIGHT, dimension.height);
      element->set(YARS_STRING_DEPTH,  dimension.depth);
    };

    static void set(RHDimension &dimension, DataParseElement *element)
    {
      element->set(YARS_STRING_RADIUS, dimension.radius);
      element->set(YARS_STRING_HEIGHT, dimension.height);
    };

};

#endif // __DATA_DIMENSION_FACTORY_H__


