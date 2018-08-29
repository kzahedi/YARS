#include <yars/configuration/data/DataScreen.h>

#include <yars/configuration/data/DataPoseFactory.h>
#include <yars/configuration/data/DataScaleFactory.h>
#include <yars/configuration/data/DataCamera.h>
#include <yars/configuration/data/DataColourFactory.h>

#include <yars/configuration/YarsConfiguration.h>

# define YARS_STRING_TEXTURES                      (char*)"textures"
# define YARS_STRING_WINDOW_SIZE                   (char*)"size"
# define YARS_STRING_WIDTH                         (char*)"width"
# define YARS_STRING_HEIGHT                        (char*)"height"
# define YARS_STRING_WIDTH_HEIGHT_DEFINITION       (char*)"width_height_definition"
# define YARS_STRING_AUTO_SHOW                     (char*)"show"
# define YARS_STRING_AUTO_CAPTURE                  (char*)"capture"
# define YARS_STRING_AUTO_FOLLOW                   (char*)"follow"
# define YARS_STRING_POSITION                      (char*)"position"
# define YARS_STRING_POSITION_DEFINITION           (char*)"position_definition"
# define YARS_STRING_POSITION                      (char*)"position"
# define YARS_STRING_LOOK_AT                       (char*)"lookAt"
# define YARS_STRING_SCALE                         (char*)"scale"
# define YARS_STRING_TRUE_FALSE_DEFINITION         (char*)"true_false_definition"
# define YARS_STRING_SCREEN_TEXTURES_DEFINITION    (char*)"screenTextures_definition"
# define YARS_STRING_XYZ_DEFINITION                (char*)"xyz_definition"
# define YARS_STRING_NAME                          (char*)"name"
# define YARS_STRING_POSE                          (char*)"pose"
# define YARS_STRING_POSE_DEFINITION               (char*)"pose_definition"
# define YARS_STRING_XY_DEFINITION                 (char*)"xy_definition"
# define YARS_STRING_OSD                           (char*)"osd"
# define YARS_STRING_OSD_DEFINITION                (char*)"osd_definition"
# define YARS_STRING_OSD_ROBOT                     (char*)"robot"
# define YARS_STRING_OSD_TIME                      (char*)"time"
# define YARS_STRING_OSD_FONT_DEFINITION           (char*)"osd_font_definition"
# define YARS_STRING_OSD_FONT_SIZE                 (char*)"size"
# define YARS_STRING_OSD_FONT_WIDTH                (char*)"width"
# define YARS_STRING_OSD_FONT_HEIGHT               (char*)"height"
# define YARS_STRING_OSD_FONT_TYPE                 (char*)"font"
# define YARS_STRING_OSD_FONT_COLOR                (char*)"colour"
# define YARS_STRING_XSD_POSITIVE_NON_ZERO_INTEGER (char*)"[1-9][0-9]*"
# define YARS_STRING_POSITIVE_NON_ZERO_INTEGER     (char*)"positive_non_zero_integer_definition"
# define YARS_STRING_COLOR                         (char*)"colour"
# define YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION  (char*)"colour_hex_rgba_definition"
# define YARS_STRING_XSD_HEX_COLOR                 (char*)"[A-Fa-f0-9]{6}([A-Fa-f0-9]{2}){0,1}"


# define YARS_STRING_WINDOW_SIZE_OPTIONS    (char*)"window_size_option_definitions"
# define YARS_STRING_VISUALISATION_OPTION_1 (char*)"window_size_option_1"
# define YARS_STRING_VISUALISATION_OPTION_2 (char*)"window_size_option_2"
# define YARS_STRING_RESOLUTION             (char*)"resolution"
# define YARS_STRING_RESOLUTION_DEFINITION  (char*)"resolution_definition"
# define YARS_STRING_RESOLUTION_LIST        (char*)"resolution_list_definition"

# define YARS_STRING_SD                     (char*)"sd"
# define YARS_STRING_XGA                    (char*)"xga"
# define YARS_STRING_720                    (char*)"720p"
# define YARS_STRING_1080                   (char*)"1080p"
# define YARS_STRING_VGA                    (char*)"vga"  // 640x480
# define YARS_STRING_SVGA                   (char*)"svga" // 800x600


DataScreen::DataScreen(DataNode *parent)
  : DataNode(parent)
{
  _name             = "YARS";
  _autoShow         = true;
  _autoCapture      = __YARS_GET_CAPTURE_OPTION_SET;
  _autoFollow       = true;
  _camera           = new DataCamera(this);

  _fontTimeName     = "Time";
  _fontTimeSize     = 24;
  _fontTimeColour    = Colour(46.0f/255.0f, 72.0f/255.0f, 92.0f/255.0f, 1.f);
  _fontTimeWidth    = 200;
  _fontTimeHeight   = 100;

  _fontRobotName    = "Robot";
  _fontRobotSize    = 16;
  _fontRobotColour   = Colour(46.0f/255.0f, 72.0f/255.0f, 92.0f/255.0f, 1.f);
  _fontRobotWidth   = 200;
  _fontRobotHeight  = 100;

  _screenGeometry.setHeight(480);
  _screenGeometry.setWidth(640);
  _screenGeometry.setX(SG_CENTER);     // TODO TODO
  _screenGeometry.setY(SG_CENTER);
}

DataScreen::~DataScreen()
{
  // nothing to be done
}

string DataScreen::name()
{
  return _name;
}

bool DataScreen::autoShow()
{
  return _autoShow;
}

bool DataScreen::autoCapture()
{
  return _autoCapture;
}

bool DataScreen::autoFollow()
{
  return _autoFollow;
}

ScreenGeometry DataScreen::geometry()
{
  return _screenGeometry;
}

DataCamera* DataScreen::camera()
{
  return _camera;
}

Axes DataScreen::axes()
{
  return _axes;
}

void DataScreen::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_SCREEN))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_SCREEN))
  {
    element->set(YARS_STRING_NAME,         _name);
    element->set(YARS_STRING_AUTO_SHOW,    _autoShow);
    element->set(YARS_STRING_AUTO_CAPTURE, _autoCapture);
    element->set(YARS_STRING_AUTO_FOLLOW,  _autoFollow);
    _autoCapture |= __YARS_GET_CAPTURE_OPTION_SET;
  }
  if(element->opening(YARS_STRING_WINDOW_SIZE))
  {
    if(element->hasAttriute(YARS_STRING_HEIGHT))
    {
      int height; int width;
      element->set(YARS_STRING_HEIGHT, height);
      element->set(YARS_STRING_WIDTH,  width);
      _screenGeometry.setWidth(width);
      _screenGeometry.setHeight(height);
    }
  }
  if(element->opening(YARS_STRING_RESOLUTION))
  {
    string s;
    element->set(YARS_STRING_NAME, s);
    if(s == YARS_STRING_SD)
    {
      _screenGeometry.setWidth(768);
      _screenGeometry.setHeight(576);
    }
    else if(s == YARS_STRING_XGA)
    {
      _screenGeometry.setWidth(1024);
      _screenGeometry.setHeight(768);
    }
    else if(s == YARS_STRING_720)
    {
      _screenGeometry.setWidth(1280);
      _screenGeometry.setHeight(720);
    }
    else if(s == YARS_STRING_1080)
    {
      _screenGeometry.setWidth(1920);
      _screenGeometry.setHeight(1080);
    }
    else if(s == YARS_STRING_VGA)
    {
      _screenGeometry.setWidth(640);
      _screenGeometry.setHeight(480);
    }
    else if(s == YARS_STRING_SVGA)
    {
      _screenGeometry.setWidth(800);
      _screenGeometry.setHeight(600);
    }
    else
    {
      YarsErrorHandler::push("Unknown resolution given");
      exit(-1);
    }
  }
  if(element->opening(YARS_STRING_POSITION))
  {
    int x; int y;
    element->set(YARS_STRING_X, x);
    element->set(YARS_STRING_Y, y);
    _screenGeometry.setXY(x, y);
  }
  // if(element->opening(YARS_STRING_AXIS))
  // {
  //   Axis a;
  //   _axes.push_back(a);
  // }
  if(element->opening(YARS_STRING_CAMERA))
  {
    current = _camera;
    _camera->add(element);
  }

  if(element->opening(YARS_STRING_OSD_TIME))
  {
    string color = "";
    element->set(YARS_STRING_OSD_FONT_TYPE,  _fontTimeName);
    element->set(YARS_STRING_OSD_FONT_SIZE,  _fontTimeSize);
    element->set(YARS_STRING_OSD_FONT_COLOR,  color);
    if(color != "") DataColourFactory::set(_fontTimeColour, color);
  }

  if(element->opening(YARS_STRING_OSD_ROBOT))
  {
    string color = "";
    element->set(YARS_STRING_OSD_FONT_TYPE,  _fontRobotName);
    element->set(YARS_STRING_OSD_FONT_SIZE,  _fontRobotSize);
    element->set(YARS_STRING_OSD_FONT_COLOR,  color);
    if(color != "") DataColourFactory::set(_fontRobotColour, color);
  }

  if(element->opening(YARS_STRING_POSE))
  {
    DataPoseFactory::set(_axes[_axes.size() - 1], element);
  }

  if(element->opening(YARS_STRING_SCALE))
  {
    DataScaleFactory::set(_axes[_axes.size() - 1].scale, element);
  }
}

void DataScreen::createXsd(XsdSpecification *spec)
{
  XsdChoice *sizeOptions = new XsdChoice(YARS_STRING_WINDOW_SIZE_OPTIONS, 1, 1);
  sizeOptions->add(NE(YARS_STRING_RESOLUTION,  YARS_STRING_RESOLUTION_DEFINITION,   0, 1));
  sizeOptions->add(NE(YARS_STRING_WINDOW_SIZE, YARS_STRING_WIDTH_HEIGHT_DEFINITION, 0, 1));

  XsdSequence *screenDefinition = new XsdSequence(YARS_STRING_SCREEN_DEFINITION);
  screenDefinition->add(NA(YARS_STRING_NAME,         YARS_STRING_XSD_STRING,            true));
  screenDefinition->add(NA(YARS_STRING_AUTO_SHOW,    YARS_STRING_TRUE_FALSE_DEFINITION, false));
  screenDefinition->add(NA(YARS_STRING_AUTO_CAPTURE, YARS_STRING_TRUE_FALSE_DEFINITION, false));
  screenDefinition->add(NA(YARS_STRING_AUTO_FOLLOW,  YARS_STRING_TRUE_FALSE_DEFINITION, false));
  screenDefinition->add(sizeOptions);
  screenDefinition->add(NE(YARS_STRING_POSITION,     YARS_STRING_XY_DEFINITION,              0, 1));
  screenDefinition->add(NE(YARS_STRING_CAMERA,       YARS_STRING_SCREEN_CAMERA_DEFINITION,   0, 1));
  screenDefinition->add(NE(YARS_STRING_OSD,          YARS_STRING_OSD_DEFINITION,             0, 1));
  // screenDefinition->add(NE(YARS_STRING_AXIS,         YARS_STRING_AXIS_DEFINITION,            0));
  spec->add(screenDefinition);

  // XsdSequence *axisDefinition = new XsdSequence(YARS_STRING_AXIS_DEFINITION);
  // axisDefinition->add(NE(YARS_STRING_POSE,  YARS_STRING_POSE_DEFINITION, 1, 1));
  // axisDefinition->add(NE(YARS_STRING_SCALE, YARS_STRING_XYZ_DEFINITION,  1, 1));
  // spec->add(axisDefinition);


  // XsdSequence *option1 = new XsdSequence(YARS_STRING_VISUALISATION_OPTION_1);
  // option1->add(NE(YARS_STRING_WINDOW_SIZE,  YARS_STRING_RESOLUTION_DEFINITION,  0, 1));
  // option1->add(NE(YARS_STRING_WINDOW_SIZE, YARS_STRING_WIDTH_HEIGHT_DEFINITION, 0, 1));
  // sizeOptions->add(option1);

  XsdSequence *resolutionDefinition = new XsdSequence(YARS_STRING_RESOLUTION_DEFINITION);
  resolutionDefinition->add(NA(YARS_STRING_NAME, YARS_STRING_RESOLUTION_LIST, true));
  spec->add(resolutionDefinition);

  XsdEnumeration *sizeResolutions = new XsdEnumeration(YARS_STRING_RESOLUTION_LIST, YARS_STRING_XSD_STRING);
  sizeResolutions->add(YARS_STRING_SD);
  sizeResolutions->add(YARS_STRING_XGA);
  sizeResolutions->add(YARS_STRING_720);
  sizeResolutions->add(YARS_STRING_1080);
  sizeResolutions->add(YARS_STRING_VGA);
  sizeResolutions->add(YARS_STRING_SVGA);
  spec->add(sizeResolutions);

  // XsdSequence *option2 = new XsdSequence(YARS_STRING_VISUALISATION_OPTION_2);
  // option2->add(NE(YARS_STRING_WINDOW_SIZE, YARS_STRING_WIDTH_HEIGHT_DEFINITION, 1, 1));

  // sizeOptions->add(option2);

  XsdSequence *widthHeightDefinition = new XsdSequence(YARS_STRING_WIDTH_HEIGHT_DEFINITION);
  widthHeightDefinition->add(NA(YARS_STRING_WIDTH,  YARS_STRING_XSD_INTEGER, true));
  widthHeightDefinition->add(NA(YARS_STRING_HEIGHT, YARS_STRING_XSD_INTEGER, true));
  spec->add(widthHeightDefinition);

  XsdSequence *osdDefinition = new XsdSequence(YARS_STRING_OSD_DEFINITION);
  osdDefinition->add(NE(YARS_STRING_OSD_TIME,  YARS_STRING_OSD_FONT_DEFINITION, 0, 1));
  osdDefinition->add(NE(YARS_STRING_OSD_ROBOT, YARS_STRING_OSD_FONT_DEFINITION, 0, 1));
  spec->add(osdDefinition);

  XsdSequence *fontDefinition = new XsdSequence(YARS_STRING_OSD_FONT_DEFINITION);
  fontDefinition->add(NA(YARS_STRING_OSD_FONT_TYPE,   YARS_STRING_XSD_STRING,                   false));
  fontDefinition->add(NA(YARS_STRING_OSD_FONT_SIZE,   YARS_STRING_POSITIVE_NON_ZERO_INTEGER,    false));
  fontDefinition->add(NA(YARS_STRING_OSD_FONT_COLOR,  YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION, false));
  fontDefinition->add(NA(YARS_STRING_OSD_FONT_WIDTH,  YARS_STRING_POSITIVE_NON_ZERO_INTEGER,    false));
  fontDefinition->add(NA(YARS_STRING_OSD_FONT_HEIGHT, YARS_STRING_POSITIVE_NON_ZERO_INTEGER,    false));
  spec->add(fontDefinition);

  XsdRegularExpression *nonZeroPositiveInteger =
    new XsdRegularExpression(YARS_STRING_POSITIVE_NON_ZERO_INTEGER,
                             YARS_STRING_XSD_INTEGER, YARS_STRING_XSD_POSITIVE_NON_ZERO_INTEGER);
  spec->add(nonZeroPositiveInteger);

  XsdRegularExpression *colorHex =
    new XsdRegularExpression(YARS_STRING_COLOR_HEX_REG_EXP_DEFINITION,
                             YARS_STRING_XSD_STRING, YARS_STRING_XSD_HEX_COLOR);
  spec->add(colorHex);


  DataCamera::createXsd(spec);

}


DataScreen* DataScreen::copy()
{
  DataScreen *copy       = new DataScreen(NULL);
  copy->_name            = _name;
  copy->_axes            = _axes;
  copy->_screenGeometry  = _screenGeometry;
  copy->_autoShow        = _autoShow;
  copy->_autoCapture     = _autoCapture;
  copy->_autoFollow      = _autoFollow;
  copy->_camera          = _camera->copy();
  copy->_fontRobotColour  = _fontRobotColour;
  copy->_fontRobotName   = _fontRobotName;
  copy->_fontRobotSize   = _fontRobotSize;
  copy->_fontRobotWidth  = _fontRobotWidth;
  copy->_fontRobotHeight = _fontRobotHeight;
  copy->_fontTimeColour   = _fontTimeColour;
  copy->_fontTimeName    = _fontTimeName;
  copy->_fontTimeSize    = _fontTimeSize;
  copy->_fontTimeWidth   = _fontTimeWidth;
  copy->_fontTimeHeight  = _fontTimeHeight;
  return copy;
}

int DataScreen::osdRobotFontSize()
{
  return _fontRobotSize;
}

string DataScreen::osdRobotFontName()
{
  return _fontRobotName;
}

Colour DataScreen::osdRobotFontColour()
{
  return _fontRobotColour;
}

int DataScreen::osdTimeFontSize()
{
  return _fontTimeSize;
}

string DataScreen::osdTimeFontName()
{
  return _fontTimeName;
}

Colour DataScreen::osdTimeFontColour()
{
  return _fontTimeColour;
}

int DataScreen::osdRobotFontHeight()
{
  return _fontRobotHeight;
}

int DataScreen::osdRobotFontWidth()
{
  return _fontRobotWidth;
}

int DataScreen::osdTimeFontHeight()
{
  return _fontTimeHeight;
}

int DataScreen::osdTimeFontWidth()
{
  return _fontTimeWidth;
}

