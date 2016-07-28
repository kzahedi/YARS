#ifndef __DATA_ORBIT_CAM_H__
#define __DATA_ORBIT_CAM_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/util/PID.h>

# define YARS_STRING_ORBIT_CAM            (char*)"orbit"
# define YARS_STRING_ORBIT_CAM_DEFINITION (char*)"orbit" DIVIDER "cam" DIVIDER "config" DIVIDER DEFINITION

class DataOrbitCam : public DataNode
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataOrbitCam(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataOrbitCam();

    void add(DataParseElement *element);

    DataOrbitCam* copy();

    static void createXsd(XsdSpecification *spec);

    PID lookAtX();
    PID lookAtY();
    PID lookAtZ();

    PID fromX();
    PID fromY();
    PID fromZ();

    yReal speed();

  private:


    PID _lookAtX;
    PID _lookAtY;
    PID _lookAtZ;

    PID _fromX;
    PID _fromY;
    PID _fromZ;

    yReal _speed;

};

#endif // __DATA_ORBIT_CAM_H__


