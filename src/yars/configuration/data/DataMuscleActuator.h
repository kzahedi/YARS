#ifndef __DATA_MUSCLE_ACTUATOR__
#define __DATA_MUSCLE_ACTUATOR__

#include "yars/configuration/data/DataActuator.h"
#include "yars/types/Domain.h"
#include "yars/util/Mapping.h"
// TODO: documentation

#define YARS_STRING_MUSCLE (char*)"muscle"
#define YARS_STRING_MUSCLE_DEFINITION YARS_STRING_MUSCLE DIVIDER "definition"

class DataMuscleActuator : public DataActuator
{
  public:
    // TODO: Try with reference
    DataMuscleActuator(DataNode* parent);

    yReal velocity() const;
    yReal force() const;

    void add(DataParseElement* element) override;

    // TODO: Most should be const. But we would have to change super classes.
    void applyOffset(Pose pose) override;
    std::string source() override;
    std::string destination() const;
    std::string name() override;

    void setInternalValue(int index, yReal value) override;
    void setExternalValue(int index, yReal value) override;
    yReal internalValue(int index) override;
    yReal externalValue(int index) override;
    int dimension() override;
    void setDesiredValue(int index, yReal value) override;
    yReal getInternalDesiredValue(int index) override;
    yReal getExternalDesiredValue(int index) override;

    Domain getInternalDomain(int index) override;
    Domain getExternalDomain(int index) override;
    bool isActive(int index) override;

    yReal getAppliedForce(int index) override;
    yReal getAppliedVelocity(int index) override;

    void setAppliedForceAndVelocity(int index, yReal force, yReal velocity)
      override;

    Pose pose() override;
    DataActuator* _copy() override;

    static void createXsd(XsdSpecification& spec);

  private:
    void close();
    void setMapping();

    // TODO: Decide what's needed.
    //DataFilter     *_filter;
    //DataNoise      *_noise;
    //Domain          _deflection;
    Domain          _mapping;
    //SliderParameter _parameter;
    //Pose            _pose;
    //bool            _deflectionSet;
    std::string _source;
    std::string _destination;
    std::string _jointType;
    //string          _mode;
    std::string _name;

    std::vector<yReal> _internalValue;
    std::vector<yReal> _externalValue;
    std::vector<yReal> _desiredValue;
    std::vector<yReal> _desiredExValue;
    std::vector<Mapping> _internalExternalMapping;
    std::vector<Domain> _internalDomain;
    std::vector<Domain> _externalDomain;
    //Noise          *_n;
    //bool            _isActive;
    //yReal           _currentTransitionalVelocity;
    //bool            _poseInWorldCoordinates;
    //yReal           _appliedForce;
    //yReal           _appliedVelocity;
    //yReal           _friction;
    //pthread_mutex_t _mutex;
    
    yReal _maxForce;
    yReal _maxVelocity;
    yReal _forceScaling;
};

#endif // __DATA_MUSCLE_ACTUATOR__
