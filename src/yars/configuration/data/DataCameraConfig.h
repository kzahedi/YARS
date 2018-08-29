#ifndef __DATA_CAMERA_CONFIG_H__
#define __DATA_CAMERA_CONFIG_H__

# define YARS_STRING_CAMERA_CONFIG            (char*)"cameras"
# define YARS_STRING_CAMERA_CONFIG_DEFINITION (char*)"camera_config_definition"

#include "DataNode.h"
#include "DataCameraConfig.h"

#include "DataOrbitCam.h"
#include "DataCenterOrbitCam.h"
#include "DataOffsetCam.h"

#include <string>

using namespace std;

class DataCameraConfig : public DataNode
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataCameraConfig(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataCameraConfig();

    void add(DataParseElement *element);

    DataCameraConfig* copy();

    DataOrbitCam*       orbit();
    DataCenterOrbitCam* center();
    DataOffsetCam*      offset();

    static void createXsd(XsdSpecification *spec);

  private:

    DataOrbitCam       *_orbit;
    DataCenterOrbitCam *_center;
    DataOffsetCam      *_offset;
};



#endif // __DATA_CAMERA_CONFIG_H__
