#ifndef __DATA_MUSCLE_ACTUATOR__
#define __DATA_MUSCLE_ACTUATOR__

#include "yars/configuration/data/DataActuator.h"
// TODO: documentation

class DataMuscleActuator : public DataActuator
{
  public:
    // TODO: Try with reference
    DataMuscleActuator(DataNode* parent);

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

    const std::string _destination;

};

#endif // __DATA_MUSCLE_ACTUATOR__
