#ifndef __DATA_GENERIC_ACTUATOR_H__
#define __DATA_GENERIC_ACTUATOR_H__

#include <yars/configuration/data/DataActuator.h>
#include <yars/configuration/data/DataNoise.h>
#include <yars/configuration/data/DataFilter.h>
#include <yars/types/Domain.h>
#include <yars/types/Pose.h>

#include <yars/util/PID.h>
#include <yars/util/Mapping.h>
#include <yars/util/noise/Noise.h>

#include <pthread.h>

# define GENERIC                        "generic"
# define YARS_STRING_GENERIC            (char*)GENERIC
# define YARS_STRING_GENERIC_DEFINITION (char*)GENERIC DIVIDER "actuator" DIVIDER DEFINITION

class TypeParameter
{
  public:
    yReal softness;
    yReal damping;
    yReal restitution;
    bool useResitution;
    bool useDamping;
    bool useSoftness;

    TypeParameter()
    {
      softness        = 0.0;
      damping         = 0.0;
      restitution     = 0.0;
      useResitution   = false;
      useDamping      = false;
      useSoftness     = false;
    }
    TypeParameter & operator=(const TypeParameter &t)
    {
      softness        = t.softness;
      damping         = t.damping;
      restitution     = t.restitution;
      useResitution   = t.useResitution;
      useDamping      = t.useDamping;
      useSoftness     = t.useSoftness;
      return *this;
    };
    friend std::ostream& operator<<(std::ostream& str, const TypeParameter& p)
    {
      str << " Softness:    " << p.softness      << " (" << (p.useSoftness?"true":"false")   << ")" << endl;
      str << " Damping:     " << p.damping       << " (" << (p.useDamping?"true":"false")    << ")" << endl;
      str << " Restitution: " << p.restitution   << " (" << (p.useResitution?"true":"false") << ")" << endl;
      return str;
    }
};

class AxisParameter
{
  public:
    string        type;
    string        mode;
    yReal         maxForce;
    yReal         forceScaling;
    yReal         velocity;
    yReal         friction;
    Domain        mapping;
    Domain        deflection;
    bool          isDeflectionSet;
    bool          isActive;
    bool          isDefined;
    DataNoise*    noise;
    DataFilter*   filter;
    PID           pid;
    Domain        internalDomain;
    Domain        externalDomain;
    Mapping       internalExternalMapping;
    int           controlType;
    Noise*        n;
    yReal         internalValue;
    yReal         externalValue;
    yReal         inDesiredValue;
    yReal         exDesiredValue;
    TypeParameter parameter;

    yReal         springDamping;
    yReal         springStiffness;

    AxisParameter()
    {
      maxForce        = -1.0;
      forceScaling    =  0.0;
      velocity        = -1.0;
      friction        =  0.0;
      isDeflectionSet = false;
      isActive        = false;
      isDefined       = false;
      noise           = NULL;
      filter          = NULL;
      controlType     = -1;
      n               = NULL;
      internalValue   = 0.0;
      externalValue   = 0.0;
      inDesiredValue  = 0.0;
      exDesiredValue  = 0.0;
      // useSpring       = false;
      springDamping   = 0.0;
      springStiffness = 1.0;
    };

    AxisParameter & operator=(const AxisParameter &a)
    {
      type            = a.type;
      mode            = a.mode;
      maxForce        = a.maxForce;
      forceScaling    = a.forceScaling;
      velocity        = a.velocity;
      friction        = a.friction;
      mapping         = a.mapping;
      deflection      = a.deflection;
      isDeflectionSet = a.isDeflectionSet;
      isActive        = a.isActive;
      isDefined       = a.isDefined;
      pid             = a.pid;
      controlType     = a.controlType;
      parameter       = a.parameter;
      internalDomain  = a.internalDomain;
      externalDomain  = a.externalDomain;
      // useSpring       = a.useSpring;
      springDamping   = a.springDamping;
      springStiffness = a.springStiffness;
      if(a.noise  != NULL) noise  = a.noise->copy();
      if(a.filter != NULL) filter = a.filter->copy();
      return *this;
    };

    friend std::ostream& operator<<(std::ostream& str, const AxisParameter& p)
    {
      str << " Active:            " << (p.isActive?"true":"false") << endl;
      str << " Type:              " << p.type         << endl;
      str << " Control Type:      " << p.controlType  << endl;
      str << " Mode:              " << p.mode         << endl;
      str << " Force:             " << p.maxForce     << endl;
      str << " Force scaling      " << p.forceScaling << endl;
      str << " Velocity:          " << p.velocity     << endl;
      str << " Friction:          " << p.friction     << endl;
      str << " Mapping:           " << p.mapping      << endl;
      str << " Deflection:        " << p.deflection   << " (set = " << (p.isDeflectionSet?"true":"false") << ")"
                                                      << endl;
      str << " PID:               " << p.pid          << endl;
      // str << " Use spring:        " << p.useSpring   << endl;
      str << " Spring parameter:  " << p.springDamping << ", " << p.springStiffness << endl;
      str << " Parameter:         " << endl << p.parameter << endl;
      return str;
    };

};

class DOFParameter
{
  public:
    TypeParameter parameter;
    AxisParameter x;
    AxisParameter y;
    AxisParameter z;

    DOFParameter& operator=(const DOFParameter &d)
    {
      parameter = d.parameter;
      x = d.x;
      y = d.y;
      z = d.z;
      return *this;
    }

    friend std::ostream& operator<<(std::ostream& str, const DOFParameter& p)
    {
      str << "Parameter: " << endl << p.parameter << endl;
      str << "X: " << endl << p.x << endl;
      str << "Y: " << endl << p.y << endl;
      str << "Z: " << endl << p.z << endl;
      return str;
    };

};

class GenericParamter
{
  public:
    DOFParameter rotational;
    DOFParameter translational;
    // bool usesSprings;

    GenericParamter & operator=(const GenericParamter &g)
    {
      rotational    = g.rotational;
      translational = g.translational;
      // usesSprings   = g.usesSprings;
      return *this;
    }

    friend std::ostream& operator<<(std::ostream& str, const GenericParamter& p)
    {
      str << endl << endl;
      // str << "Uses springs:"  << p.usesSprings?"true":"false" << endl;
      str << "Rotational:"    << endl << p.rotational    << endl;
      str << "Translational:" << endl << p.translational << endl;
      return str;
    };

};

class DataGenericActuator : public DataActuator
{
  public:
    DataGenericActuator(DataNode *parent);
    ~DataGenericActuator();

    void add(DataParseElement *element);

    string         name();
    string         source();
    string         destination();
    // P3D            anchor();
    // P3D            axis();
    Pose            pose();
    GenericParamter parameter();

    void applyOffset(Pose offset);
    static void createXsd(XsdSpecification *spec);

    DataGenericActuator* _copy();

    yReal velocity(int index);
    yReal force(int index);
    void setVelocity(int index, yReal v);
    void setForce(int index, yReal f);
    yReal internalValue(int index);
    yReal externalValue(int index);
    void setInternalValue(int index, yReal v);
    void setExternalValue(int index, yReal v);
    Domain getInternalDomain(int index);
    Domain getExternalDomain(int index);
    void setDesiredValue(int index, yReal value);
    yReal getInternalDesiredValue(int index);
    yReal getExternalDesiredValue(int index);

    int dimension();
    bool isActive(int index);
    vector<int> mapping();


    void  setAngularVelocity(int index, yReal v);
    yReal getAngularVelocity(int index);
    void  setLinearVelocity(int index, yReal v);
    yReal getLinearVelocity(int index);

    void  setAngle(int index, yReal v);
    yReal getAngle(int index);
    void  setPosition(int index, yReal v);
    yReal getPosition(int index);

    yReal springDamping(int index);
    yReal springStiffness(int index);
    // bool  useSpring(int index);

    yReal getAppliedForce(int);
    yReal getAppliedVelocity(int);
    void setAppliedForceAndVelocity(int, yReal, yReal);

    bool usesSprings();
    int mapping(int);

  private:
    void             __setParameters(AxisParameter &p, DataParseElement *element);
    void             __setMapping();
    void             __setDomains(AxisParameter    &parameter);
    void             __setSpringParameters(AxisParameter &p, DataParseElement *element);
    void             __setAxisParameters(TypeParameter &p, DataParseElement *element);
    void             __setAxisSpringParameters(AxisParameter &p, DataParseElement *element);
    void             __setAxisForceParameters(AxisParameter &p, DataParseElement *element);
    void             __setAxisVelocityParameters(AxisParameter &p, DataParseElement *element);
    Pose             _pose;
    GenericParamter  _parameter;
    string           _destination;
    string           _source;
    string           _name;
    bool             _parsingRotational;
    bool             _parsingTranslational;
    bool             _parsingXAxis;
    bool             _parsingYAxis;
    bool             _parsingZAxis;
    bool             _useSpring;
    int              _dimension;
    vector<int>      _mapping;
    vector<yReal>    _angularVelocity;
    vector<yReal>    _linearVelocity;
    vector<yReal>    _angle;
    vector<yReal>    _position;
    vector<yReal>    _appliedForce;
    vector<yReal>    _appliedVelocity;
    bool             _poseInWorldCoordinates;

    pthread_mutex_t _mutex;
};

#endif // __DATA_GENERIC_ACTUATOR_H__


