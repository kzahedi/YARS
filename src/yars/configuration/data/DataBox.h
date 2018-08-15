#ifndef __DATA_BOX_H__
#define __DATA_BOX_H__

#include <yars/configuration/data/DataObject.h>
#include <yars/configuration/data/DataNode.h>
#include <yars/configuration/data/DataPhysicsParameter.h>

#include <yars/types/WHDDimension.h>
#include <yars/types/Pose.h>

# define YARS_STRING_OBJECT_BOX                   (char*)"box"
# define YARS_STRING_OBJECT_BOX_DEFINTION         (char*)"object_box_definition"
# define YARS_STRING_BOX_VISUALISATION            (char*)"visualisation"
# define YARS_STRING_BOX_VISUALISATION_DEFINITION (char*)"box_visualisation_definition"

/**
 * @brief Contains all information about a box.
 */
class DataBox : public DataObject
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataBox(DataNode *parent);

    /**
     * @brief Destructor.
     */
    ~DataBox();

    /**
     * @brief Returns the dimension of the box, defined by width, heigh, and
     * depth.
     *
     * @return dimension
     * @sa WHDDimension
     */
    WHDDimension dimension();

    string texture(int index);
    void setTexture(int index, string texture);


    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    void setDimension(WHDDimension dimension);

    DataBox* _copy();
    void     _resetTo(const DataObject* other);

    static void createXsd(XsdSpecification *spec);


  private:
    WHDDimension   _dimension;
    std::vector<string> _texture;
};

#endif // __DATA_BOX_H__
