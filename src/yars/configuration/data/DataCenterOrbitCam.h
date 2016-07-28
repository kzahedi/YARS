#ifndef __DATA_CENTER_ORBIT_CAM_H__
#define __DATA_CENTER_ORBIT_CAM_H__

#include "DataNode.h"

# define YARS_STRING_CENTER_ORBIT_CAM            (char*)"centre"
# define YARS_STRING_CENTER_ORBIT_CAM_DEFINITION (char*)"centre" DIVIDER "cam" DIVIDER "config" DIVIDER DEFINITION

class DataCenterOrbitCam : public DataNode
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataCenterOrbitCam(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataCenterOrbitCam();

    void add(DataParseElement *element);

    DataCenterOrbitCam* copy();

    static void createXsd(XsdSpecification *spec);

    yReal speed();

  private:

    yReal _speed;

};

#endif // __DATA_CENTER_ORBIT_CAM_H__


