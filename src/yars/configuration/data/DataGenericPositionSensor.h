#ifndef __DATA_GENERIC_POSITION_SENSOR_H__
#define __DATA_GENERIC_POSITION_SENSOR_H__

#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>
#include <yars/types/Domain.h>

# define YARS_STRING_GENERIC_POSITION_SENSOR            (char*)"position"
# define YARS_STRING_GENERIC_POSITION_SENSOR_DEFINITION (char*)"position_definition"

#include <string>
#include <pthread.h>

using namespace std;

class DataGenericPositionSensor : public DataSensor
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataGenericPositionSensor(DataNode* parent);

    /**
     * @brief Destructor.
     */
    ~DataGenericPositionSensor();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    bool x();
    bool y();
    bool z();

    int dimension();

    static void createXsd(XsdSpecification *spec);

    DataGenericPositionSensor *_copy();

    double internalValue(int index);
    double externalValue(int index);
    void setInternalValue(int index, double v);
    void setExternalValue(int index, double v);
    Domain getInternalDomain(int index) { return Domain(__Y_REAL_MIN, __Y_REAL_MAX); };
    Domain getExternalDomain(int index) { return Domain(__Y_REAL_MIN, __Y_REAL_MAX); };

    void _resetTo(const DataSensor *sensor);

  private:
    bool      _x;
    bool      _y;
    bool      _z;
    int       _dimension;

    double* _value;
    pthread_mutex_t _mutex;
};

#endif // __DATA_GENERIC_POSITION_SENSOR_H__

