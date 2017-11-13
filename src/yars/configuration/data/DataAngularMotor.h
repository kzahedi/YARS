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

    double velocity();
    double force();
    void setVelocity(double v);
    void setForce(double f);
    double internalValue(int index);
    double externalValue(int index);
    void setInternalValue(int index, double v);
    void setExternalValue(int index, double v);
    void setDesiredValue(int index, double v);
    double getInternalDesiredValue(int index);
    double getExternalDesiredValue(int index);
    double getCurrentAngularVelocity();
    void  setCurrentAngularVelocity(double v);

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

    double       _internalValue;
    double       _externalValue;
    double       _desiredValue;
    double       _desiredExValue;
    Mapping     _internalExternalMapping;
    Domain      _internalDomain;
    Domain      _externalDomain;
    Noise      *_n;
    bool        _isActive;
    double       _currentAngularVelocity;
    bool        _poseInWorldCoordinates;

    pthread_mutex_t _mutex;
};

#endif // __DATA_ANGULAR_MOTOR_H__
