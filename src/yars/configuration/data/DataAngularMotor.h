#ifndef __DATA_ANGULAR_MOTOR_H__
#define __DATA_ANGULAR_MOTOR_H__

#include "DataActuator.h"
#include "DataNoise.h"
#include "DataFilter.h"
#include "types/Domain.h"
#include "types/Pose.h"
#include "util/Mapping.h"
#include "util/PID.h"
#include "util/noise/Noise.h"

# define ANGULAR_MOTOR                        "hinge"
# define YARS_STRING_ANGULAR_MOTOR            (char*)ANGULAR_MOTOR
# define YARS_STRING_ANGULAR_MOTOR_DEFINITION (char*)ANGULAR_MOTOR DIVIDER DEFINITION

class DataAngularMotor : public DataActuator
{
  public:
    DataAngularMotor(DataNode *parent);
    ~DataAngularMotor();

    void add(DataParseElement *element);

    string name();
    string source();
    string destination();
    string jointType();
    Domain deflection();
    Pose   pose();
    Domain mapping();
    DataNoise* noise();
    DataFilter* filter();
    HingeParameter parameter();
    string mode();

    yReal velocity();
    yReal force();
    void setVelocity(yReal v);
    void setForce(yReal f);
    yReal internalValue(int index);
    yReal externalValue(int index);
    void setInternalValue(int index, yReal v);
    void setExternalValue(int index, yReal v);
    void setDesiredValue(int index, yReal v);
    yReal getInternalDesiredValue(int index);
    yReal getExternalDesiredValue(int index);
    yReal getCurrentAngularVelocity();
    void  setCurrentAngularVelocity(yReal v);

    Domain getInternalDomain(int index);
    Domain getExternalDomain(int index);
    bool isActive(int index);

    int dimension() { return 1; }

    bool isDeflectionSet();
    void applyOffset(Pose offset);

    static void createXsd(XsdSpecification *spec);

    DataAngularMotor* _copy();

  private:
    void __close();
    void __setMapping();

    DataFilter    *_filter;
    DataNoise     *_noise;
    Domain         _deflection;
    Domain         _mapping;
    HingeParameter  _parameter;
    Pose           _pose;
    bool           _deflectionSet;
    string         _destination;
    string         _jointType;
    string         _mode;
    string         _name;
    string         _source;

    yReal       _internalValue;
    yReal       _externalValue;
    yReal       _desiredValue;
    yReal       _desiredExValue;
    Mapping     _internalExternalMapping;
    Domain      _internalDomain;
    Domain      _externalDomain;
    Noise      *_n;
    bool        _isActive;
    yReal       _currentAngularVelocity;
    bool        _poseInWorldCoordinates;

    pthread_mutex_t _mutex;
};

#endif // __DATA_ANGULAR_MOTOR_H__
