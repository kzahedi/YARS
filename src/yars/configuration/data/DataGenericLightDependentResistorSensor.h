#ifndef __DATA_GENERIC_LIGHT_DEPENDENT_RESISTOR_SENSOR_H__
#define __DATA_GENERIC_LIGHT_DEPENDENT_RESISTOR_SENSOR_H__

#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>

#include <yars/types/Colour.h>
#include <yars/types/P2D.h>
#include <yars/types/Pose.h>

#include <yars/types/Domain.h>
#include <yars/util/Mapping.h>
#include <yars/util/noise/Noise.h>

#include <pthread.h>

# define GENERIC_LDR_SENSOR "ldr"
# define YARS_STRING_GENERIC_LDR_SENSOR  (char*)GENERIC_LDR_SENSOR
# define YARS_STRING_GENERIC_LDR_SENSOR_DEFINITION  (char*)GENERIC_LDR_SENSOR DIVIDER DEFINITION

#include <string>

using namespace std;

class DataGenericLightDependentResistorSensor : public DataSensor
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataGenericLightDependentResistorSensor(DataNode* parent);

    /**
     * @brief Destructor.
     */
    ~DataGenericLightDependentResistorSensor();

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
    double opening();

    /**
     * @brief Returns the domain of the sensor, i.e. which light values can be
     * measured.
     *
     * @return domain
     */
    Colour colour();

    static void createXsd(XsdSpecification *spec);

    DataGenericLightDependentResistorSensor *_copy();

    int dimension() { return 1; } // not implemented yet

    double internalValue(int index);
    double externalValue(int index);
    void setInternalValue(int index, double v);
    void setExternalValue(int index, double v);
    Domain getInternalDomain(int index);
    Domain getExternalDomain(int index);

    void _resetTo(const DataSensor *sensor);

  private:
    void __setMapping();
    Pose            _pose;
    double           _openingAngle;
    Colour          _colour;

    double           _internalValue;
    double           _externalValue;
    Mapping         _internalExternalMapping;
    Domain          _internalDomain;
    Domain          _externalDomain;
    pthread_mutex_t _mutex;
    Noise*          _n;

};

#endif // __DATA_GENERIC_LIGHT_DEPENDENT_RESISTOR_SENSOR_H__
