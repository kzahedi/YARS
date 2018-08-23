#ifndef __DATA_MUSCLE_H__

#define __DATA_MUSCLE_H__

#include <yars/configuration/data/DataActuator.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>
#include <yars/configuration/data/DataObjects.h>
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
      optimalLength        = 0.0;
      maxVelocity          = 0.0;
      N                    = -1.0;
      K                    = -1.0;
      w                    = -1.0;
      c                    = -1.0;
      maxForce             = -1.0;
      useLengthComponent   = false;
      useVelocityComponent = false;
      softnessDir          = -1.0;
      restitutionDir       = -1.0;
      dampingDir           = -1.0;
      softnessLim          = -1.0;
      restitutionLim       = -1.0;
      dampingLim           = -1.0;
      softnessOrtho        = -1.0;
      restitutionOrtho     = -1.0;
      dampingOrtho         = -1.0;
    }

    MuscleParameter& operator=(const MuscleParameter &p)
    {
      optimalLength        = p.optimalLength;
      maxVelocity          = p.maxVelocity;
      N                    = p.N;
      K                    = p.K;
      w                    = p.w;
      c                    = p.c;
      maxForce             = p.maxForce;
      useLengthComponent   = p.useLengthComponent;
      useVelocityComponent = p.useVelocityComponent;
      softnessDir          = p.softnessDir;
      restitutionDir       = p.restitutionDir;
      dampingDir           = p.dampingDir;
      softnessLim          = p.softnessLim;
      restitutionLim       = p.restitutionLim;
      dampingLim           = p.dampingLim;
      softnessOrtho        = p.softnessOrtho;
      restitutionOrtho     = p.restitutionOrtho;
      dampingOrtho         = p.dampingOrtho;

      return *this;
    };

    double optimalLength;
    double maxVelocity;
    double N;
    double K;
    double w;
    double c;
    double maxForce;
    bool useLengthComponent;
    bool useVelocityComponent;
    double softnessDir;
    double restitutionDir;
    double dampingDir;
    double softnessLim;
    double restitutionLim;
    double dampingLim;
    double softnessOrtho;
    double restitutionOrtho;
    double dampingOrtho;
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
    Domain deflection();
    Pose   pose();
    Domain mapping();
    DataNoise* noise();
    DataFilter* filter();
    MuscleParameter parameter();
    string mode();
    string type();
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
    void   setAppliedForceAndVelocity(int index, double force, double velocity);

    double friction();

    Domain getInternalDomain(int index);
    Domain getExternalDomain(int index);
    bool isActive(int index);

    double lengthComponentW();
    double lengthComponentC();
    double lengthComponentOptimalLength();
    bool   lengthComponentUse();

    double velocityComponentN();
    double velocityComponentK();
    double velocitComponentMaxVelocity();
    double erp();
    double cfm();
    bool   velocityComponentUse();

    void setLengthComponent(double fl);
    void setVelocityComponent(double fv);
    void setMuscleLength(double l);
    void setMuscleVelocity(double  v);

    double getVelocityComponent();
    double getLengthComponent();
    double getMuscleLength();
    double getMuscleVelocity();

    int dimension() { return 1; }

    bool isDeflectionSet();
    void applyOffset(Pose offset);

    static void createXsd(XsdSpecification *spec);

    DataObject* sourceAnchor();
    DataObject* destinationAnchor();

    DataActuator* sourceConnector();
    DataActuator* destinationConnector();

    DataMuscleActuator* _copy();

  private:
    void __close();
    void __setMapping();

    DataFilter*     _filter;
    DataNoise*      _noise;
    Domain          _deflection;
    Domain          _mapping;
    MuscleParameter _parameter;
    bool            _deflectionSet;
    string          _destination;
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
    double          _currentTransitionalVelocity;
    double          _appliedForce;
    double          _appliedVelocity;
    Pose            _srcOffset;
    bool            _srcOffsetInWorldCoordinates;
    Pose            _dstOffset;
    bool            _dstOffsetInWorldCoordinates;
    bool            _visualise;
    pthread_mutex_t _mutex;
    DataObject*     _srcObject;
    DataObject*     _dstObject;
    DataActuator*   _srcConnector;
    DataActuator*   _dstConnector;
    bool            _parsingSourceAnchor;
    bool            _parsingDestinationAnchor;
    double          _friction;

    double          _lengthComponentW;
    double          _lengthComponentC;
    double          _lengthComponentOptimalLength;
    bool            _lengthComponentUse;

    double          _velocityComponentN;
    double          _velocityComponentK;
    double          _velocityComponentMaxVelocity;
    bool            _velocityComponentUse;

    double          _erp;
    double          _cfm;

    double          _fl;
    double          _fv;
    double          _muscleLength;
    double          _muscleVelocity;
};

#endif // __DATA_MUSCLE_H__
