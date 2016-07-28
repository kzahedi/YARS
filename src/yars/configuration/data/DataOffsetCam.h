#ifndef __DATA_OFFSET_CAM_H__
#define __DATA_OFFSET_CAM_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/util/PID.h>

# define YARS_STRING_OFFSET_CAM            (char*)"offset"
# define YARS_STRING_OFFSET_CAM_DEFINITION (char*)"offset" DIVIDER "cam" DIVIDER "config" DIVIDER DEFINITION

class DataOffsetCam : public DataNode
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataOffsetCam(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataOffsetCam();

    void add(DataParseElement *element);

    DataOffsetCam* copy();

    static void createXsd(XsdSpecification *spec);

    PID lookAtX();
    PID lookAtY();
    PID lookAtZ();

    PID fromX();
    PID fromY();
    PID fromZ();

  private:

    PID _lookAtX;
    PID _lookAtY;
    PID _lookAtZ;

    PID _fromX;
    PID _fromY;
    PID _fromZ;
};

#endif // __DATA_OFFSET_CAM_H__


