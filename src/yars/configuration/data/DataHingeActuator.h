#ifndef __DATA_HINGE_H__
#define __DATA_HINGE_H__

#include <yars/configuration/data/DataActuator.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>
#include <yars/types/Domain.h>
#include <yars/types/Pose.h>
#include <yars/util/Mapping.h>
#include <yars/util/PID.h>
#include <yars/util/noise/Noise.h>

#include <pthread.h>

# define HINGE                        "hinge"
# define YARS_STRING_HINGE            (char*)HINGE
# define YARS_STRING_HINGE_DEFINITION (char*)HINGE DIVIDER DEFINITION

class HingeParameter
{
  public:
    HingeParameter()
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
    }

    HingeParameter& operator=(const HingeParameter &p)
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

    PID pid;
};



class DataHingeActuator : public DataActuator
{
  public:
    DataHingeActuator(DataNode *parent);
    ~DataHingeActuator();

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

    int dimension();

    bool isDeflectionSet();
    void applyOffset(Pose offset);

    yReal friction();

    yReal getAppliedForce(int index);
    yReal getAppliedVelocity(int index);
    void setAppliedForceAndVelocity(int index, yReal force, yReal velocity);

    static void createXsd(XsdSpecification *spec);

    DataHingeActuator* _copy();

  private:
    void __close();
    void __setMapping();

    DataFilter     *_filter;
    DataNoise      *_noise;
    Domain          _deflection;
    Domain          _mapping;
    HingeParameter  _parameter;
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
    yReal           _currentAngularVelocity;
    bool            _poseInWorldCoordinates;
    yReal           _appliedForce;
    yReal           _appliedVelocity;
    yReal           _friction;
    pthread_mutex_t _mutex;

};

#endif // __DATA_HINGE_H__
