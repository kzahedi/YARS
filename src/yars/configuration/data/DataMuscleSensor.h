#ifndef __DATA_MUSCLE_SENSOR_H__
#define __DATA_MUSCLE_SENSOR_H__

#include <yars/configuration/data/DataSensor.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>

#include <yars/types/Domain.h>

#include <yars/util/Mapping.h>

# define YARS_STRING_MUSCLE_SENSOR            (char*)"muscle"
# define YARS_STRING_MUSCLE_SENSOR_DEFINITION (char*)"muscle_sensor_definition"


#include <string>

using namespace std;

class DataMuscleSensor : public DataSensor
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataMuscleSensor(DataNode* parent);

    /**
     * @brief Destructor.
     */
    ~DataMuscleSensor();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    static void createXsd(XsdSpecification *spec);

    DataMuscleSensor *_copy();

    int dimension() { return _dimension; }

    double internalValue(int index);
    double externalValue(int index);
    void setInternalValue(int index, double v);
    void setExternalValue(int index, double v);

    Domain getInternalDomain(int index);
    Domain getExternalDomain(int index);

    void _resetTo(const DataSensor *sensor);

    void setValues(double fv, double fl, double l, double muscleVelocity, double f, double v);

    double appliedForce();
    double appliedVelocity();
    double lengthComponent();
    double velocityComponent();
    double muscleLength();
    double muscleVelocity();
    Domain mapping(int i);
    Domain domain(int i);
    

  private:
    void __setMapping();
    vector<Domain> _domain;
    vector<Domain> _mapping;
    vector<double> _internalValue;
    vector<double> _externalValue;
    vector<Mapping> _internalExternalMapping;
    vector<Domain> _internalDomain;
    vector<Domain> _externalDomain;
    int _dimension;
    int _dmIndex;

    // double _f;              - 0
    // double _fv;             - 1
    // double _fl;             - 2
    // double _muscleVelocity; - 3
    // double _length;         - 4
    // double _velocity;       - 5
};

#endif // __DATA_MUSCLE_SENSOR_H__
