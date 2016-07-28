#ifndef __DATA_GENERIC_CAMERA_SENSOR_H__
#define __DATA_GENERIC_CAMERA_SENSOR_H__

#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>

#include <yars/types/Domain.h>
#include <yars/types/Pose.h>
#include <yars/types/P2D.h>

# define GENERIC_CAMERA_SENSOR                        "camera"
# define YARS_STRING_GENERIC_CAMERA_SENSOR  (char*)GENERIC_CAMERA_SENSOR
# define YARS_STRING_GENERIC_CAMERA_SENSOR_DEFINITION  (char*)GENERIC_CAMERA_SENSOR DIVIDER DEFINITION


class DataGenericCameraSensor : public DataSensor
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataGenericCameraSensor(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataGenericCameraSensor();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    /**
     * @brief Returns the pose with respect to the origin of the attaching object.
     *
     * @return
     */
    Pose pose();

    /**
     * @brief Returns the opening angles in x and y direction.
     *
     * @return opening angles
     * @sa P2D
     */
    P2D openingAngles();

    static void createXsd(XsdSpecification *spec);

    DataGenericCameraSensor* _copy();

    int dimension() { return 0; } // not implemented yet

    yReal internalValue(int index) {return 0; };
    yReal externalValue(int index) {return 0; };
    void setInternalValue(int index, yReal v) { };
    void setExternalValue(int index, yReal v) { };
    Domain getInternalDomain(int index) { return Domain(); };
    Domain getExternalDomain(int index) { return Domain(); };

    void _resetTo(const DataSensor *sensor);

  private:
    Pose        _pose;
    P2D         _openingAngles;
};

#endif // __DATA_GENERIC_CAMERA_SENSOR_H__
