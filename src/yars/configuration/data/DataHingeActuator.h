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

    double softnessDir;
    double restitutionDir;
    double dampingDir;

    double softnessLim;
    double restitutionLim;
    double dampingLim;

    double softnessOrtho;
    double restitutionOrtho;
    double dampingOrtho;

    double maxVelocity;
    double maxForce;

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

    int dimension();

    bool isDeflectionSet();
    void applyOffset(Pose offset);

    double friction();

    double getAppliedForce(int index);
    double getAppliedVelocity(int index);
    void setAppliedForceAndVelocity(int index, double force, double velocity);

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

    vector<double>   _internalValue;
    vector<double>   _externalValue;
    vector<double>   _desiredValue;
    vector<double>   _desiredExValue;
    vector<Mapping> _internalExternalMapping;
    vector<Domain>  _internalDomain;
    vector<Domain>  _externalDomain;
    Noise          *_n;
    bool            _isActive;
    double           _currentAngularVelocity;
    bool            _poseInWorldCoordinates;
    double           _appliedForce;
    double           _appliedVelocity;
    double           _friction;
    pthread_mutex_t _mutex;

};

#endif // __DATA_HINGE_H__
