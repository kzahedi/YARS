#ifndef __DATA_FIXED_H__
#define __DATA_FIXED_H__

#include "DataActuator.h"

# define FIXED                        "fixed"
# define YARS_STRING_FIXED            (char*)FIXED
# define YARS_STRING_FIXED_DEFINITION (char*)FIXED DIVIDER DEFINITION

#include <string>

using namespace std;

class DataFixedActuator : public DataActuator
{
  public:
    DataFixedActuator(DataNode *parent);

    void add(DataParseElement *element);
    string name();
    string source();
    string destination();

    static void createXsd(XsdSpecification *spec);
    void applyOffset(Pose offset);

    DataFixedActuator* _copy();

    void setInternalValue(int, yReal)  { };
    void setExternalValue(int, yReal)  { };
    yReal internalValue(int)           { return 0.0; };
    yReal externalValue(int)           { return 0.0; };
    Domain getInternalDomain(int)      { return Domain(); };
    Domain getExternalDomain(int)      { return Domain(); };
    void setDesiredValue(int, yReal)   { };
    yReal getInternalDesiredValue(int) { return (yReal)0.0;};
    yReal getExternalDesiredValue(int) { return 0.0; };

    int dimension()                    { return 0; }
    bool isActive(int)                 { return false; }

    Pose pose()                        { return Pose(); }
    void setCurrentPosition(P3D)       { };
    void setCurrentOrientation(P3D)    { };
    P3D  getCurrentPosition()          { return P3D(); };
    P3D  getCurrentOrientation()       { return P3D(); };

    yReal getAppliedForce(int)                         {return 0.0;};
    yReal getAppliedVelocity(int)                      {return 0.0;};
    void setAppliedForceAndVelocity(int, yReal, yReal) {};


  private:
    string _name;
    string _source;
    string _destination;
};

#endif // __DATA_FIXED_H__
