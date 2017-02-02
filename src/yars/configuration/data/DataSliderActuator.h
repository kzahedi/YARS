#ifndef __DATA_SLIDER_H__
#define __DATA_SLIDER_H__

#include <yars/configuration/data/DataActuator.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>
#include <yars/types/Domain.h>
#include <yars/types/Pose.h>
#include <yars/util/Mapping.h>
#include <yars/util/PID.h>
#include <yars/util/noise/Noise.h>

#include <pthread.h>

# define SLIDER                        "slider"
# define YARS_STRING_SLIDER            (char*)SLIDER
# define YARS_STRING_SLIDER_DEFINITION (char*)SLIDER DIVIDER DEFINITION

class SliderParameter
{
  public:
    SliderParameter()
    {
      softnessDir      = -1.0;
      restitutionDir   = -1.0;
      dampingDir       = -1.0;

      softnessLim      = -1.0;
      restitutionLim   = -1.0;
      dampingLim       = -1.0;

      softnessOrtho    = -1.0;
      restitutionOrtho = -1.0;
      dampingOrtho     = -1.0;

      maxVelocity      =  0.0;
      maxForce         =  0.0;
      forceScaling     = -1.0;
    }

    SliderParameter& operator=(const SliderParameter &p)
    {
      softnessDir      = p.softnessDir;
      restitutionDir   = p.restitutionDir;
      dampingDir       = p.dampingDir;

      softnessLim      = p.softnessLim;
      restitutionLim   = p.restitutionLim;
      dampingLim       = p.dampingLim;

      softnessOrtho    = p.softnessOrtho;
      restitutionOrtho = p.restitutionOrtho;
      dampingOrtho     = p.dampingOrtho;

      maxVelocity      = p.maxVelocity;
      maxForce         = p.maxForce;
      forceScaling     = p.forceScaling;

      pid              = p.pid;

      return *this;
    };

    yReal softnessDir;
    yReal restitutionDir;
    yReal dampingDir;

    yReal softnessLim;
    yReal restitutionLim;
    yReal dampingLim;

    yReal softnessOrtho;
    yReal restitutionOrtho;
    yReal dampingOrtho;

    yReal maxVelocity;
    yReal maxForce;
    yReal forceScaling;

    PID pid;
};



class DataSliderActuator : public DataActuator
{
  public:
    DataSliderActuator(DataNode *parent);
    ~DataSliderActuator();

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
    SliderParameter parameter();
    string mode();
    void setPosition(P3D position);

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
    yReal getCurrentTransitionalVelocity();
    void  setCurrentTransitionalVelocity(yReal v);

    yReal getAppliedForce(int index);
    yReal getAppliedVelocity(int index);
    void setAppliedForceAndVelocity(int index, yReal force, yReal velocity);

    yReal friction();

    Domain getInternalDomain(int index);
    Domain getExternalDomain(int index);
    bool isActive(int index);

    int dimension() { return 1; }

    bool isDeflectionSet();
    void applyOffset(Pose offset);

    static void createXsd(XsdSpecification *spec);

    DataSliderActuator* _copy();

  private:
    void __close();
    void __setMapping();

    DataFilter     *_filter;
    DataNoise      *_noise;
    Domain          _deflection;
    Domain          _mapping;
    SliderParameter _parameter;
    Pose            _pose;
    bool            _deflectionSet;
    string          _destination;
    string          _jointType;
    string          _mode;
    string          _name;
    string          _source;

    vector<yReal>   _internalValue;
    vector<yReal>   _externalValue;
    vector<yReal>   _desiredValue;
    vector<yReal>   _desiredExValue;
    vector<Mapping> _internalExternalMapping;
    vector<Domain>  _internalDomain;
    vector<Domain>  _externalDomain;
    Noise          *_n;
    bool            _isActive;
    yReal           _currentTransitionalVelocity;
    bool            _poseInWorldCoordinates;
    yReal           _appliedForce;
    yReal           _appliedVelocity;
    yReal           _friction;
    pthread_mutex_t _mutex;

};

#endif // __DATA_SLIDER_H__
