#ifndef __DATA_MUSCLE_H__
#define __DATA_MUSCLE_H__

#include <yars/configuration/data/DataActuator.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>
#include <yars/types/Domain.h>
#include <yars/types/Pose.h>
#include <yars/util/Mapping.h>
#include <yars/util/PID.h>
#include <yars/util/noise/Noise.h>

#include <pthread.h>

# define YARS_STRING_MUSCLE            (char*)"muscle"
# define YARS_STRING_MUSCLE_DEFINITION (char*)"muscle_definition"

class MuscleParameter
{
  public:
    MuscleParameter()
    {
      maxVelocity      =  0.0;
      maxForce         =  0.0;
      forceScaling     = -1.0;
    }

    MuscleParameter& operator=(const MuscleParameter &p)
    {
      maxVelocity      = p.maxVelocity;
      maxForce         = p.maxForce;
      forceScaling     = p.forceScaling;

      return *this;
    };

    double maxVelocity;
    double maxForce;
    double forceScaling;
};



class DataMuscleActuator : public DataActuator
{
  public:
    DataMuscleActuator(DataNode *parent);
    ~DataMuscleActuator();

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
    MuscleParameter parameter();
    string mode();
    void setPosition(P3D position);

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
    double getCurrentTransitionalVelocity();
    void  setCurrentTransitionalVelocity(double v);

    double getAppliedForce(int index);
    double getAppliedVelocity(int index);
    void setAppliedForceAndVelocity(int index, double force, double velocity);

    double friction();

    Domain getInternalDomain(int index);
    Domain getExternalDomain(int index);
    bool isActive(int index);

    int dimension() { return 1; }

    bool isDeflectionSet();
    void applyOffset(Pose offset);

    static void createXsd(XsdSpecification *spec);

    DataMuscleActuator* _copy();

  private:
    void __close();
    void __setMapping();

    DataFilter*     _filter;
    DataNoise*      _noise;
    Domain          _deflection;
    Domain          _mapping;
    MuscleParameter _parameter;
    Pose            _pose;
    bool            _deflectionSet;
    string          _destination;
    string          _jointType;
    string          _mode;
    string          _name;
    string          _source;

    vector<double>  _internalValue;
    vector<double>  _externalValue;
    vector<double>  _desiredValue;
    vector<double>  _desiredExValue;
    vector<Mapping> _internalExternalMapping;
    vector<Domain>  _internalDomain;
    vector<Domain>  _externalDomain;
    Noise*          _n;
    bool            _isActive;
    double          _currentTransitionalVelocity;
    bool            _poseInWorldCoordinates;
    double          _appliedForce;
    double          _appliedVelocity;
    double          _friction;
    pthread_mutex_t _mutex;

};

#endif // __DATA_MUSCLE_H__
