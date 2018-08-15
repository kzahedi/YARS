#ifndef __DATA_CYLINDER_H__
#define __DATA_CYLINDER_H__

#include <yars/configuration/data/DataObject.h>
#include <yars/types/Pose.h>
#include <yars/types/RHDimension.h>

# define YARS_STRING_OBJECT_CYLINDER            (char*)"cylinder"
# define YARS_STRING_OBJECT_CYLINDER_DEFINTION  (char*)"object_cylinder_definition"
# define YARS_STRING_C_VISUALISATION            (char*)"visualisation"
# define YARS_STRING_C_VISUALISATION_DEFINITION (char*)"capsule_visualisation_definition"

#include <string>

using namespace std;

/**
 * @brief Contains all data for a cyliner
 */
class DataCylinder : public DataObject
{
  public:

    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataCylinder(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataCylinder();

    /**
     * @brief Returns the radius and height of the cylinder.
     *
     * @return dimension
     * @sa RHDimension
     */
    RHDimension dimension();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    static void createXsd(XsdSpecification *spec);

    DataCylinder* _copy();

    void setDimension(RHDimension dimension);

    void _resetTo(const DataObject* other);

    string texture(int index);
    void setTexture(int index, string texture);

  protected:
    DataCylinder(DataNode *parent, int type);

    RHDimension    _dimension;
    std::vector<string> _texture;
};

#endif // __DATA_CYLINDER_H__
