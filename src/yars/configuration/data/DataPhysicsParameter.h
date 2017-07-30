#ifndef __DATA_PHYSICS_PARAMETER_H__
#define __DATA_PHYSICS_PARAMETER_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/defines/types.h>
#include <yars/types/P3D.h>

#include <float.h>

# define YARS_STRING_PHYSICS                     (char*)"physics"
# define YARS_STRING_PHYSICS_DEFINITION          (char*)"physics"         DIVIDER DEFINITION

# define YARS_STRING_MASS                        (char*)"mass"
# define YARS_STRING_MASS_DEFINITION             (char*)"mass"            DIVIDER DEFINITION
# define YARS_STRING_KG                          (char*)"kg"

# define YARS_STRING_FRICTION                    (char*)"friction"
# define YARS_STRING_FRICTION_DEFINITION         (char*)"friction"        DIVIDER DEFINITION
# define YARS_STRING_LINEAR_DAMPING              (char*)"linearDamping"
# define YARS_STRING_LINEAR_DAMPING_DEFINITION   (char*)"linearDamping"   DIVIDER DEFINITION
# define YARS_STRING_ANGULAR_DAMPING             (char*)"angularDamping"
# define YARS_STRING_ANGULAR_DAMPING_DEFINITION  (char*)"angularDamping"  DIVIDER DEFINITION
# define YARS_STRING_ROLLING_FRICTION            (char*)"rollingFriction"
# define YARS_STRING_ROLLING_FRICTION_DEFINITION (char*)"rollingFriction" DIVIDER DEFINITION
# define YARS_STRING_RESTITUTION                 (char*)"restitution"
# define YARS_STRING_RESTITUTION_DEFINITION      (char*)"restitution"     DIVIDER DEFINITION



class DataPhysicsParameter : public DataNode
{
  public:

    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataPhysicsParameter(DataNode *parent);

    /**
     * @brief Destructor.
     */
    virtual ~DataPhysicsParameter();
    void add(DataParseElement *element);

    double   linearDamping();
    double   mass();
    double   angularDamping();
    double   friction();
    double   rollingFriction();
    double   restitution();
    P3D        centreOfMass();

    void setMass(double mass);

    bool      useLinearDamping();
    bool      useAngularDamping();
    bool      useFriction();
    bool      useRollingFriction();
    bool      useRestitution();


    static void createXsd(XsdSpecification *spec);
    DataPhysicsParameter* copy();

  private:
    double  _linearDamping;
    double  _mass;
    double  _angularDamping;
    double  _friction;
    double  _rollingFriction;
    double  _restitution;
    P3D       _centreOfMass;

    bool      _useLinearDamping;
    bool      _useAngularDamping;
    bool      _useFriction;
    bool      _useRollingFriction;
    bool      _useRestitution;

};

#endif // __DATA_PHYSICS_PARAMETER_H__
