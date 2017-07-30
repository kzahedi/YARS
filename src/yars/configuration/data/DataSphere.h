#ifndef __DATA_SPHERE_H__
#define __DATA_SPHERE_H__

#include <yars/defines/types.h>
#include <yars/types/Pose.h>

#include <yars/configuration/data/DataPhysicsParameter.h>
#include <yars/configuration/data/DataObject.h>

// #include "DataVisualisation.h"

# define SPHERE                              "sphere"
# define YARS_STRING_OBJECT_SPHERE           (char*)SPHERE
# define YARS_STRING_OBJECT_SPHERE_DEFINTION (char*)"object" DIVIDER SPHERE DIVIDER DEFINITION

# define YARS_STRING_SPHERE_VISUALISATION            (char*)"visualisation"
# define YARS_STRING_SPHERE_VISUALISATION_DEFINITION (char*)"sphere" DIVIDER "visualisation" DIVIDER DEFINITION

#include <string>

using namespace std;

/**
 * @brief Contains all data for a sphere.
 */
class DataSphere : public DataObject
{
  public:

    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataSphere(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataSphere();

    /**
     * @brief Returns the radius of this sphere.
     *
     * @return radius
     */
    double radius();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataSphere* _copy();
    void _resetTo(const DataObject* other);

    static void createXsd(XsdSpecification *spec);
    void setRadius(double radius);

    string texture();
    void setTexture(string texture);


  private:
    double _radius;
    string   _texture;
};

#endif // __DATA_SPHERE_H__
