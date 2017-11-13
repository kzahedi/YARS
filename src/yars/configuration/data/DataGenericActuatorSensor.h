#ifndef __DATA_GENERIC_ACTUATOR_SENSOR_H__
#define __DATA_GENERIC_ACTUATOR_SENSOR_H__

#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>

#include <yars/types/Domain.h>

#include <yars/util/Mapping.h>
#include <yars/util/noise/Noise.h>

#include <pthread.h>


# define GENERIC_ACTUATOR_SENSOR_X          1
# define GENERIC_ACTUATOR_SENSOR_Y          2
# define GENERIC_ACTUATOR_SENSOR_Z          4

# define GENERIC_ACTUATOR_SENSOR_DEFLECTION 8
# define GENERIC_ACTUATOR_SENSOR_VELOCITY   16

# define GENERIC_ACTUATOR_SENSOR_LINEAR     32
# define GENERIC_ACTUATOR_SENSOR_ANGULAR    64


# define GENERIC_ACTUATOR_SENSOR_X_DEFLECTION_ANGULAR \
  GENERIC_ACTUATOR_SENSOR_X          | \
  GENERIC_ACTUATOR_SENSOR_DEFLECTION | \
  GENERIC_ACTUATOR_SENSOR_ANGULAR

# define GENERIC_ACTUATOR_SENSOR_X_VELOCITY_ANGULAR \
  GENERIC_ACTUATOR_SENSOR_X          | \
  GENERIC_ACTUATOR_SENSOR_VELOCITY   | \
  GENERIC_ACTUATOR_SENSOR_ANGULAR

# define GENERIC_ACTUATOR_SENSOR_Y_DEFLECTION_ANGULAR \
  GENERIC_ACTUATOR_SENSOR_Y          | \
  GENERIC_ACTUATOR_SENSOR_DEFLECTION | \
  GENERIC_ACTUATOR_SENSOR_ANGULAR

# define GENERIC_ACTUATOR_SENSOR_Y_VELOCITY_ANGULAR \
  GENERIC_ACTUATOR_SENSOR_Y          | \
  GENERIC_ACTUATOR_SENSOR_VELOCITY   | \
  GENERIC_ACTUATOR_SENSOR_ANGULAR

# define GENERIC_ACTUATOR_SENSOR_Z_DEFLECTION_ANGULAR \
  GENERIC_ACTUATOR_SENSOR_Z          | \
  GENERIC_ACTUATOR_SENSOR_DEFLECTION | \
  GENERIC_ACTUATOR_SENSOR_ANGULAR

# define GENERIC_ACTUATOR_SENSOR_Z_VELOCITY_ANGULAR \
  GENERIC_ACTUATOR_SENSOR_Z          | \
  GENERIC_ACTUATOR_SENSOR_VELOCITY   | \
  GENERIC_ACTUATOR_SENSOR_ANGULAR

# define GENERIC_ACTUATOR_SENSOR_X_DEFLECTION_LINEAR \
  GENERIC_ACTUATOR_SENSOR_X          | \
  GENERIC_ACTUATOR_SENSOR_DEFLECTION | \
  GENERIC_ACTUATOR_SENSOR_LINEAR 

# define GENERIC_ACTUATOR_SENSOR_X_VELOCITY_LINEAR \
  GENERIC_ACTUATOR_SENSOR_X          | \
  GENERIC_ACTUATOR_SENSOR_VELOCITY   | \
  GENERIC_ACTUATOR_SENSOR_LINEAR

# define GENERIC_ACTUATOR_SENSOR_Y_DEFLECTION_LINEAR \
  GENERIC_ACTUATOR_SENSOR_Y          | \
  GENERIC_ACTUATOR_SENSOR_DEFLECTION | \
  GENERIC_ACTUATOR_SENSOR_LINEAR

# define GENERIC_ACTUATOR_SENSOR_Y_VELOCITY_LINEAR \
  GENERIC_ACTUATOR_SENSOR_Y          | \
  GENERIC_ACTUATOR_SENSOR_VELOCITY   | \
  GENERIC_ACTUATOR_SENSOR_LINEAR

# define GENERIC_ACTUATOR_SENSOR_Z_DEFLECTION_LINEAR \
  GENERIC_ACTUATOR_SENSOR_Z          | \
  GENERIC_ACTUATOR_SENSOR_DEFLECTION | \
  GENERIC_ACTUATOR_SENSOR_LINEAR

# define GENERIC_ACTUATOR_SENSOR_Z_VELOCITY_LINEAR \
  GENERIC_ACTUATOR_SENSOR_Z          | \
  GENERIC_ACTUATOR_SENSOR_VELOCITY   | \
  GENERIC_ACTUATOR_SENSOR_LINEAR

# define GENERIC_ACTUATOR_SENSOR                         "generic"
# define YARS_STRING_GENERIC_ACTUATOR_SENSOR             (char*)GENERIC_ACTUATOR_SENSOR
# define YARS_STRING_GENERIC_ACTUATOR_SENSOR_DEFINITION \
  (char*)GENERIC_ACTUATOR_SENSOR DIVIDER "sensor" DIVIDER DEFINITION

#include <string>

using namespace std;

class DataGenericActuatorSensor : public DataSensor
{
  public:

    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataGenericActuatorSensor(DataNode* parent);

    /**
     * @brief Destructor
     */
    ~DataGenericActuatorSensor();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    /**
     * @brief Returns the domain of the sensor, i.e. which light values can be
     * measured.
     *
     * @return domain
     */
    Domain domain(int index);

    static void createXsd(XsdSpecification *spec);

    DataGenericActuatorSensor* _copy();

    int dimension();

    double internalValue(int index);
    double externalValue(int index);
    void setInternalValue(int index, double v);
    void setExternalValue(int index, double v);
    void setInternalDomain(int index, Domain d);
    void setExternalDomain(int index, Domain d);
    Domain getInternalDomain(int index);
    Domain getExternalDomain(int index);

    int type(int index);

    void _resetTo(const DataSensor *sensor);
    string logName(int index);

  private:
    void __setDegToRad(int index);
    void __setMapping();
    vector<Domain>      _domain;
    vector<Domain>      _mapping;
    vector<double>       _internalValue;
    vector<double>       _externalValue;
    vector<Mapping>     _internalExternalMapping;
    vector<Domain>      _internalDomain;
    vector<Domain>      _externalDomain;
    vector<Noise*>      _n;
    vector<DataNoise*>  _noise;
    vector<DataFilter*> _filter;
    vector<int>         _types;
    int                 _dimension;
    bool                _processingXAxis;
    bool                _processingYAxis;
    bool                _processingZAxis;
    string              _currentType;
    vector<string>      _logName;

    pthread_mutex_t _mutex;
};

#endif // __DATA_GENERIC_ACTUATOR_SENSOR_H__
