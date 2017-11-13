#ifndef __DATA_POINT_CONSTRAINT__
#define __DATA_POINT_CONSTRAINT__

#include "yars/configuration/data/DataActuator.h"
#include "yars/types/Domain.h"

class Mapping;

#define YARS_STRING_POINT_CONSTRAINT (char*)"pointConstraint"
#define YARS_STRING_POINT_CONSTRAINT_DEFINITION YARS_STRING_POINT_CONSTRAINT \
  DIVIDER "definition"

//! Contains data for PointConstraint. 
/*! If you use this with MuscleActuator you have to add a 'mode="muscle"'
 * attribute to the pointConstraint tag to get the correct positioning.
 */
class DataPointConstraint : public DataActuator
{
  public:
    DataPointConstraint(DataNode* parent);
    ~DataPointConstraint();

    void add(DataParseElement* element) override;

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

    std::string mode() const;

    static void createXsd(XsdSpecification& spec);

  private:
    void close();

    Domain               _mapping;
    Pose                 _pose;

    std::string          _source;
    std::string          _destination;
    std::string          _jointType;
    std::string          _name;
    std::string          _mode;

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

    double                _forceScaling;

    double           _appliedForce;
    double           _appliedVelocity;
};

#endif // __DATA_POINT_CONSTRAINT__
