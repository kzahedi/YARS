#ifndef __DATA_CAPPED_CYLINDER_H__
#define __DATA_CAPPED_CYLINDER_H__

#include "DataCylinder.h"

# define YARS_STRING_OBJECT_CAPPED_CYLINDER           (char*)"capsule"
# define YARS_STRING_OBJECT_CAPPED_CYLINDER_DEFINTION (char*)"object_capsule_definition"
# define YARS_STRING_CC_VISUALISATION                 (char*)"visualisation"
# define YARS_STRING_CC_VISUALISATION_DEFINITION      (char*)"capsule_visualisation_definition"

/**
 * @brief Contains the data for a capped cylinder. As the parameters are
 * equivalent to those of a cylinder, this class inherits from DataCylinder
 * @sa DataCylinder
 */
class DataCapsule : public DataCylinder
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataCapsule(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataCapsule();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    string texture(int index);
    void setTexture(int index, string texture);

    static void createXsd(XsdSpecification *spec);

    void _resetTo(const DataObject* other);

  protected:
    DataCapsule* _copy();
    std::vector<string> _texture;


  private:
};

#endif // __DATA_CAPPED_CYLINDER_H__
