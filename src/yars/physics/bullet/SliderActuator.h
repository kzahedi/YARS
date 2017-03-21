#ifndef __SLIDER_ACTUATOR_H__
#define __SLIDER_ACTUATOR_H__

#include "yars/configuration/data/DataSliderActuator.h"
#include "yars/configuration/YarsConfiguration.h"
#include "yars/physics/bullet/Actuator.h"

#include <btBulletDynamicsCommon.h>

class SliderActuator : public Actuator
{
  public:
    SliderActuator(DataSliderActuator *data, Robot *robot);
    ~SliderActuator();

    void prePhysicsUpdate();
    void postPhysicsUpdate();
    void reset();
    btTypedConstraint* constraint();

    DataActuator* data();

  private:
    void __processPositional();
    void __processVelocitySlider();
    void __processForceSlider();


    DataSliderActuator *_data;
    btSliderConstraint *_sliderConstraint;
    SliderParameter     _parameter;
    int                 _sliderType;
    yReal               _position;
    yReal               _lastPosition;
    yReal               _friction;
    bool                _isActive;
    bool                _isVisualised;
    bool                _hasFriction;
};

#endif // __SLIDER_ACTUATOR_H__


