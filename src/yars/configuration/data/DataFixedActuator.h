#ifndef __DATA_FIXED_H__
#define __DATA_FIXED_H__

#include "DataActuator.h"

# define YARS_STRING_FIXED            (char*)"fixed"
# define YARS_STRING_FIXED_DEFINITION (char*)"fixed_definition"

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

    void setInternalValue(int, double)  { };
    void setExternalValue(int, double)  { };
    double internalValue(int)           { return 0.0; };
    double externalValue(int)           { return 0.0; };
    Domain getInternalDomain(int)      { return Domain(); };
    Domain getExternalDomain(int)      { return Domain(); };
    void setDesiredValue(int, double)   { };
    double getInternalDesiredValue(int) { return (double)0.0;};
    double getExternalDesiredValue(int) { return 0.0; };

    int dimension()                    { return 0; }
    bool isActive(int)                 { return false; }

    Pose pose()                        { return Pose(); }
    void setCurrentPosition(P3D)       { };
    void setCurrentOrientation(P3D)    { };
    P3D  getCurrentPosition()          { return P3D(); };
    P3D  getCurrentOrientation()       { return P3D(); };

    double getAppliedForce(int)                         {return 0.0;};
    double getAppliedVelocity(int)                      {return 0.0;};
    void setAppliedForceAndVelocity(int, double, double) {};


  private:
    string _name;
    string _source;
    string _destination;
};

#endif // __DATA_FIXED_H__
