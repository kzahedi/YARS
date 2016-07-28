#ifndef __DATA_SCREEN_H__
#define __DATA_SCREEN_H__

#include <yars/configuration/data/DataNode.h>
#include <yars/configuration/data/DataCamera.h>

#include <yars/types/ScreenGeometry.h>
#include <yars/types/Pose.h>
#include <yars/types/Axis.h>
#include <yars/types/Colour.h>
#include <yars/types/TextureDefinition.h>

# define SCREEN                                       "screen"
# define YARS_STRING_SCREEN                            (char*)SCREEN
# define YARS_STRING_SCREEN_DEFINITION                 (char*)SCREEN               DIVIDER         DEFINITION


#include <string>

using namespace std;

/**
 * @brief Screen Definition
 */
class DataScreen : public DataNode
{
  public:
    /**
     * @brief Constructor.
     *
     * @param parent
     */
    DataScreen(DataNode *parent);

    /**
     * @brief Destructor.
     */
    virtual ~DataScreen();

    /**
     * @brief Returns the name of the screen configuration.
     *
     * @return name of the screen configuration
     */
    string name();

    /**
     * @brief Returns if this screen should be opened automatically when yars is
     * started.
     *
     * @return boolean value, if this screen is automatically opened
     */
    bool autoShow();

    /**
     * @brief Returns if this screen should automatically start recording. Only active if autoShow is set to true.
     *
     * @return boolean value, if this
     */
    bool autoCapture();

    /**
     * @brief Returns if this screen should react to the --follow command line option
     *
     * @return boolean value, if this
     */
    bool autoFollow();

    /**
     * @brief Returns the screen geometry.
     *
     * Geometry includes position and dimension.
     *
     * @return screen geometry
     * @sa ScreenGeometry
     */
    ScreenGeometry geometry();

    /**
     * @brief Returns the pose of the camera.
     *
     * The pose includes the position and orientation in 3D.
     *
     * @return camera pose
     * @sa Pose
     */
    DataCamera* camera();

    /**
     * @brief Returns a vector of axis.
     *
     * Each axis is defined by a pose and scale.
     *
     * @return vector of axis
     * @sa Axes
     */
    Axes axes();

    /**
     * @brief Adds a DataParseElement to this node.
     *
     * @param element
     * @sa DataParseElement
     */
    void add(DataParseElement *element);

    DataScreen* copy();

    static void createXsd(XsdSpecification *spec);

    int    osdRobotFontSize();
    string osdRobotFontName();
    Colour osdRobotFontColour();
    int    osdRobotFontHeight();
    int    osdRobotFontWidth();

    int    osdTimeFontSize();
    string osdTimeFontName();
    Colour osdTimeFontColour();
    int    osdTimeFontHeight();
    int    osdTimeFontWidth();


  private:
    string            _name;
    Axes              _axes;
    ScreenGeometry    _screenGeometry;
    DataCamera       *_camera;
    bool              _autoShow;
    bool              _autoCapture;
    bool              _autoFollow;

    string            _fontRobotName;
    int               _fontRobotSize;
    Colour            _fontRobotColour;
    int               _fontRobotWidth;
    int               _fontRobotHeight;

    string            _fontTimeName;
    int               _fontTimeSize;
    Colour            _fontTimeColour;
    int               _fontTimeWidth;
    int               _fontTimeHeight;

};

#endif // __DATA_SCREEN_H__
