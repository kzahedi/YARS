#ifndef __DATA_AMBIENT_LIGHT_H__
#define __DATA_AMBIENT_LIGHT_H__

#include <yars/configuration/data/DataNode.h>

#include <yars/types/P3D.h>
#include <yars/types/Colour.h>
#include <yars/defines/types.h>

#include <vector>

# define YARS_STRING_AMBIENT_LIGHT            (char*)"ambientLight"
# define YARS_STRING_AMBIENT_LIGHT_DEFINITION (char*)"ambientLight_definition"

using namespace std;

class DataAmbientLight : public DataNode
{
  public:

    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataAmbientLight(DataNode *parent);

    /**
     * @brief Destructor.
     */
    virtual ~DataAmbientLight();

    /**
     * @brief Adds DataParseElement to this element. Used during parsing.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    /**
     * @brief Returns the position of the light source.
     *
     * @return position
     * @sa P3D
     */
    P3D position();

    /**
     * @brief Returns the color of the light source
     *
     * @return color
     * @sa Colour
     */
    Colour color();

    /**
     * @brief Returns the brightness.
     *
     * @return brightness
     */
    double brightness();

    DataAmbientLight *copy();

    static void createXsd(XsdSpecification *spec);

  private:
    P3D      _position;
    Colour    _color;
    double _brightness;
};

typedef vector<DataAmbientLight*> DataAmbientLights;

#endif // __DATA_AMBIENT_LIGHT_H__
