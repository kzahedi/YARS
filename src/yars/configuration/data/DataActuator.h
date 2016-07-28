#ifndef __DATA_ACTUATOR_H__
#define __DATA_ACTUATOR_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/types/Pose.h>
#include <yars/types/Domain.h>
#include <yars/defines/types.h>

#include <pthread.h>

# define DATA_ACTUATOR_HINGE              5601
# define DATA_ACTUATOR_HINGE2             5602
# define DATA_ACTUATOR_SLIDER             5603
# define DATA_ACTUATOR_FIXED              5604
# define DATA_ACTUATOR_GENERIC            5605
# define DATA_ACTUATOR_BALL               5606
# define DATA_ACTUATOR_PRISMATIC_ROTOIDE  5607
# define DATA_ACTUATOR_UNIVERSAL          5608

# define DATA_ACTUATOR_CONTROL_VELOCITY   100001
# define DATA_ACTUATOR_CONTROL_ANGULAR    100002
# define DATA_ACTUATOR_CONTROL_POSITIONAL 100003
# define DATA_ACTUATOR_CONTROL_FORCE      100004

class DataActuator : public DataNode
{
  public:
    DataActuator(DataNode *parent, int type);
    virtual ~DataActuator();

    int type();
    int controlType();

    virtual void applyOffset(Pose pose)                   = 0;
    virtual string source()                               = 0; // TODO: move from actuators here
    virtual string name()                                 = 0;

    virtual void setInternalValue(int index, yReal value) = 0;
    virtual void setExternalValue(int index, yReal value) = 0;
    virtual yReal internalValue(int index)                = 0;
    virtual yReal externalValue(int index)                = 0;
    virtual int dimension()                               = 0;
    virtual void setDesiredValue(int index, yReal value)  = 0;
    virtual yReal getInternalDesiredValue(int index)      = 0;
    virtual yReal getExternalDesiredValue(int index)      = 0;

    virtual Domain getInternalDomain(int index)           = 0;
    virtual Domain getExternalDomain(int index)           = 0;
    virtual bool isActive(int index)                      = 0;

    virtual yReal getAppliedForce(int index)              = 0;
    virtual yReal getAppliedVelocity(int index)           = 0;

    virtual void setAppliedForceAndVelocity(int index, yReal force, yReal velocity) = 0;

    virtual Pose       pose()                             = 0;
    virtual DataActuator* _copy()                         = 0;

    void       setCurrentAxisPosition(P3D position);
    void       setCurrentAxisOrientation(::Quaternion q);
    P3D        getCurrentAxisPosition();
    ::Quaternion getCurrentAxisOrientation();

    DataActuator* copy();


  protected:
    int        _controlType;
    P3D        _axisPosition;
    Quaternion _axisOrientation;

  private:
    int _type;

    pthread_mutex_t _mutex;
};

#endif // __DATA_ACTUATOR_H__
