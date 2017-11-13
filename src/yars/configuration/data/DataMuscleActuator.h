#ifndef __DATA_MUSCLE_ACTUATOR__
#define __DATA_MUSCLE_ACTUATOR__

#include "yars/configuration/data/DataActuator.h"
#include "yars/types/Domain.h"
// TODO: documentation

class Mapping;

#define YARS_STRING_MUSCLE (char*)"muscle"
#define YARS_STRING_MUSCLE_DEFINITION YARS_STRING_MUSCLE DIVIDER "definition"

class DataMuscleActuator : public DataActuator
{
  public:
    // TODO: Try with reference
    DataMuscleActuator(DataNode* parent);
    ~DataMuscleActuator();

    double velocity() const;
    double force() const;

    void add(DataParseElement* element) override;

    // TODO: Most should be const. But we would have to change super classes.
    void applyOffset(Pose pose) override;
    std::string source() override;
    std::string destination() const;
    std::string name() override;

    void setInternalValue(int index, double value) override;
    void setExternalValue(int index, double value) override;
    double internalValue(int index) override;
    double externalValue(int index) override;
    int dimension() override;
    void setDesiredValue(int index, double value) override;
    double getInternalDesiredValue(int index) override;
    double getExternalDesiredValue(int index) override;

    Domain getInternalDomain(int index) override;
    Domain getExternalDomain(int index) override;
    bool isActive(int index) override;

    double getAppliedForce(int index) override;
    double getAppliedVelocity(int index) override;

    void setAppliedForceAndVelocity(int index, double force, double velocity)
      override;

    Pose pose() override;
    DataActuator* _copy() override;

    static void createXsd(XsdSpecification& spec);

  private:
    void close();
    void setMapping();

    Domain               _mapping;
    Pose                 _pose;
    std::string          _source;
    std::string          _destination;
    std::string          _type;
    std::string          _name;

    std::vector<double>   _internalValue;
    std::vector<double>   _externalValue;
    std::vector<double>   _desiredValue;
    std::vector<double>   _desiredExValue;
    std::vector<Mapping> _internalExternalMapping;
    std::vector<Domain>  _internalDomain;
    std::vector<Domain>  _externalDomain;
    bool                 _isActive;
    bool                 _poseInWorldCoordinates;
    pthread_mutex_t      _mutex;

    double                _maxForce;
    double                _maxVelocity;
    double                _forceScaling;

    double           _appliedForce;
    double           _appliedVelocity;

    // TODO: Decide what's needed.
    //DataFilter     *_filter;
    //DataNoise      *_noise;
    //Domain          _deflection;
    //SliderParameter _parameter;
    //bool            _deflectionSet;
    //double           _currentTransitionalVelocity;
    //Noise          *_n;
    //double           _friction;
    //string          _mode;
};

#endif // __DATA_MUSCLE_ACTUATOR__
