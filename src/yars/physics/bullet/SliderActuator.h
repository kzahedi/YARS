#ifndef __SLIDER_ACTUATOR_H__
#define __SLIDER_ACTUATOR_H__

#include <yars/physics/bullet/Actuator.h>
#include <yars/configuration/YarsConfiguration.h>

#include <btBulletDynamicsCommon.h>

class SliderActuator : public Actuator
{
  public:
    SliderActuator(DataSliderActuator *data, Robot *robot);
    ~SliderActuator();

    void prePhysicsUpdate() override;
    void postPhysicsUpdate() override;
    void reset() override;
    btTypedConstraint* constraint() override;

    DataSliderActuator* data() override;

  private:
    void __processPositional();
    void __processVelocitySlider();
    void __processForceSlider();


    DataSliderActuator *_data;
    btSliderConstraint *_sliderConstraint;
    SliderParameter     _parameter;
    int                 _sliderType;
    double               _position;
    double               _lastPosition;
    double               _friction;
    bool                _isActive;
    bool                _isVisualised;
    bool                _hasFriction;
};

#endif // __SLIDER_ACTUATOR_H__


