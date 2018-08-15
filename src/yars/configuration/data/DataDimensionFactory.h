#ifndef __DATA_DIMENSION_FACTORY_H__
#define __DATA_DIMENSION_FACTORY_H__

#include <yars/types/WHDDimension.h>
#include <yars/types/RHDimension.h>

# define YARS_STRING_DEPTH                         (char*)"depth"
# define YARS_STRING_HEIGHT                        (char*)"height"
# define YARS_STRING_RADIUS                        (char*)"radius"
# define YARS_STRING_RADIUS_HEIGHT_DEFINITION      (char*)"radius_height_definition"
# define YARS_STRING_WIDTH                         (char*)"width"
# define YARS_STRING_WIDTH_HEIGHT_DEFINITION       (char*)"width_height_definition"
# define YARS_STRING_WIDTH_HEIGHT_DEPTH_DEFINITION (char*)"width_height_depth_definition"

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


