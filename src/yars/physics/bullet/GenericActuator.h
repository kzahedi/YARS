#ifndef __GENERIC_ACTUATOR_H__
#define __GENERIC_ACTUATOR_H__

#include <yars/physics/bullet/Actuator.h>
#include <yars/configuration/YarsConfiguration.h>

#include <btBulletDynamicsCommon.h>

class GenericActuator : public Actuator
{
  public:
    GenericActuator(DataGenericActuator *data, Robot *robot);
    ~GenericActuator();

    void prePhysicsUpdate();
    void postPhysicsUpdate();
    void reset();
    btTypedConstraint* constraint();

    DataGenericActuator* data();

  private:
    void __prePhysicsRot(     int axisIndex, int index, AxisParameter p);
    void __processAngular(    int axisIndex, int index, AxisParameter p);
    void __processRotVelocity(int axisIndex, int index, AxisParameter p);
    void __processRotForce(   int axisIndex, int index, AxisParameter p);

    void __prePhysicsTra(int axisIndex, int index, AxisParameter p);
    void __processPositional(int axisIndex, int index, AxisParameter p);
    void __processTraVelocity(int axisIndex, int index, AxisParameter p);
    void __processTraForce(   int axisIndex, int index, AxisParameter p);

    DataGenericActuator     *_data;
    btGeneric6DofConstraint *_genericConstraint;
    GenericParamter          _parameter;
    int                      _dimension;
    vector<int>              _rotType;
    vector<int>              _traType;
    vector<yReal>            _angle;
    vector<yReal>            _position;
    vector<yReal>            _lastAngle;
    vector<yReal>            _lastPosition;
    vector<int>              _mapping;
    btRigidBody             *_source;
    bool                     _isVisualised;
};

#endif // __GENERIC_ACTUATOR_H__



