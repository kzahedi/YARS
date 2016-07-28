#ifndef __DATA_LIGHT_SOURCE_H__
#define __DATA_LIGHT_SOURCE_H__

#include <yars/configuration/data/DataNode.h>

#include <yars/types/P3D.h>
#include <yars/types/Colour.h>
#include <yars/defines/types.h>

#include <vector>

# define LIGHT_SOURCE                        "pointLightSource"
# define YARS_STRING_LIGHT_SOURCE            (char*)LIGHT_SOURCE
# define YARS_STRING_LIGHT_SOURCE_DEFINITION (char*)LIGHT_SOURCE DIVIDER DEFINITION

using namespace std;

class DataPointLightSource : public DataNode
{
  public:

    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataPointLightSource(DataNode *parent);

    /**
     * @brief Destructor.
     */
    virtual ~DataPointLightSource();

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
    Colour colour();

    /**
     * @brief Returns the brightness.
     *
     * @return brightness
     */
    yReal brightness();

    bool draw();
    yReal size();
    string name();

    DataPointLightSource *copy();

    static void createXsd(XsdSpecification *spec);

  private:
    P3D    _position;
    Colour _color;
    yReal  _brightness;
    bool   _draw;
    yReal  _size;
    string _name;
};

typedef vector<DataPointLightSource*> DataPointLightSources;

#endif // __DATA_LIGHT_SOURCE_H__
