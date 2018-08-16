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
    void setAppliedForceAndVelocity(int index, double force, double velocity);


    Domain getInternalDomain(int index);
    Domain getExternalDomain(int index);
    bool isActive(int index);

    int dimension() { return 1; }

    bool isDeflectionSet();
    void applyOffset(Pose offset);

    static void createXsd(XsdSpecification *spec);

    DataObject* sourceObject();
    DataObject* destinationObject();

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
    vector<Domain> _internalDomain;
    vector<Domain> _externalDomain;
    Noise*         _n;
    double         _currentTransitionalVelocity;
    double         _appliedForce;
    double         _appliedVelocity;
    Pose           _srcOffset;
    bool           _srcOffsetInWorldCoordinates;
    Pose           _dstOffset;
    bool           _dstOffsetInWorldCoordinates;
    bool            _visualise;
    pthread_mutex_t _mutex;
    DataObject*     _srcObject;
    DataObject*     _dstObject;
    DataActuator*   _srcConnector;
    DataActuator*   _dstConnector;
    bool            _parsingSourceAnchor;
    bool            _parsingDestinationAnchor;

};

#endif // __DATA_MUSCLE_H__
