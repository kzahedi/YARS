#ifndef __DATA_POINT_CONSTRAINT__
#define __DATA_POINT_CONSTRAINT__

#include "yars/configuration/data/DataActuator.h"
#include "yars/types/Domain.h"
#include "yars/util/Mapping.h"
// TODO: documentation

#define YARS_STRING_POINT_CONSTRAINT (char*)"point_constraint"
#define YARS_STRING_POINT_CONSTRAINT_DEFINITION YARS_STRING_POINT_CONSTRAINT DIVIDER "definition"

class DataPointConstraint : public DataActuator
{
  public:
    // TODO: Try with reference
    DataPointConstraint(DataNode* parent);

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

    Domain               _mapping;
    Pose                 _pose;
    std::string          _source;
    std::string          _destination;
    std::string          _jointType;
    std::string          _name;

    std::vector<yReal>   _internalValue;
    std::vector<yReal>   _externalValue;
    std::vector<yReal>   _desiredValue;
    std::vector<yReal>   _desiredExValue;
    std::vector<Mapping> _internalExternalMapping;
    std::vector<Domain>  _internalDomain;
    std::vector<Domain>  _externalDomain;
    bool                 _isActive;
    bool                 _poseInWorldCoordinates;
    pthread_mutex_t      _mutex;

    yReal                _maxForce;
    yReal                _maxVelocity;
    yReal                _forceScaling;

    yReal           _appliedForce;
    yReal           _appliedVelocity;

    // TODO: Decide what's needed.
    //DataFilter     *_filter;
    //DataNoise      *_noise;
    //Domain          _deflection;
    //SliderParameter _parameter;
    //bool            _deflectionSet;
    //yReal           _currentTransitionalVelocity;
    //Noise          *_n;
    //yReal           _friction;
    //string          _mode;
};

#endif // __DATA_POINT_CONSTRAINT__
