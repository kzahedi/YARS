#ifndef __DATA_MUSCLE_ACTUATOR__
#define __DATA_MUSCLE_ACTUATOR__

#include "yars/configuration/data/DataActuator.h"
#include "yars/configuration/data/DataNode.h"

class DataMuscleActuator : public DataActuator
{
  public:
    DataMuscleActuator(DataNode* parent);
    ~DataMuscleActuator();

    void applyOffset(Pose pose);
    string source();
    string destination();
    string name();

    void setInternalValue(int index, yReal value);
    void setExternalValue(int index, yReal value);
    yReal internalValue(int index);
    yReal externalValue(int index);
    int dimension();
    void setDesiredValue(int index, yReal value);
    yReal getInternalDesiredValue(int index);
    yReal getExternalDesiredValue(int index);

    Domain getInternalDomain(int index);
    Domain getExternalDomain(int index);
    bool isActive(int index);

    yReal getAppliedForce(int index);
    yReal getAppliedVelocity(int index);

    void setAppliedForceAndVelocity(int index, yReal force, yReal velocity);

    Pose pose();
    DataActuator* _copy();

  private:
};

#endif // __DATA_MUSCLE_ACTUATOR__
