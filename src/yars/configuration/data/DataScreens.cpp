#include <yars/configuration/data/DataScreens.h>

#include <yars/configuration/YarsConfiguration.h>

# define YARS_STRING_DEFAULT                (char*)"default"
# define YARS_STRING_FOLLOWABLES            (char*)"followables"
# define YARS_STRING_FOLLOWABLES_DEFINITION (char*)"followables" DIVIDER DEFINITION
# define YARS_STRING_SKY                    (char*)"sky"
# define YARS_STRING_VISUALISE              (char*)"visualise"
# define YARS_STRING_VISUALISE_DEFINITION   (char*)"visualise"   DIVIDER DEFINITION
# define YARS_STRING_JOINTS                 (char*)"joints"
# define YARS_STRING_JOINTS_AXIS_DEFINITION (char*)"joints_visualisation" DIVIDER DEFINITION
# define YARS_STRING_HEIGHT                 (char*)"height"
# define YARS_STRING_RADIUS                 (char*)"radius"
# define YARS_STRING_NAME                   (char*)"name"
# define YARS_STRING_X                      (char*)"x"
# define YARS_STRING_Y                      (char*)"y"
# define YARS_STRING_Z                      (char*)"z"



DataScreens::DataScreens(DataNode *parent)
  : DataNode(parent)
{
  _default                    = new DataScreen(this);
  _followables                = NULL;
  _sky                        = "YARS/CloudySky";
  _cameraConfig               = new DataCameraConfig(this);
  _visualiseJoints            = false;
  _xTexture                   = "YARS/Red";
  _yTexture                   = "YARS/Green";
  _zTexture                   = "YARS/Blue";
  _jointsAxisDimension.radius = 0.1;
  _jointsAxisDimension.height = 1.0;
  _recording                  = new DataRecording(this);
}

DataScreens::~DataScreens()
{
  for(int i = 0; i < (int)size(); i++)
  {
    delete at(i);
  }
  if(_followables != NULL) delete _followables;
}

DataScreen* DataScreens::screen(int index)
{
  if(index < 0 || index >= (int)size())
  {
    return _default;
  }
  return at(index);
}

DataScreen* DataScreens::screen(string name)
{
  for(std::vector<DataScreen*>::iterator i = begin(); i != end(); i++)
  {
    if((*i)->name() == name)
    {
      return (*i);
    }
  }
  return NULL;
}

void DataScreens::add(DataParseElement *element)
{
  DataScreen *screen = NULL;
  if(element->opening(YARS_STRING_SCREEN))
  {
    screen  = new DataScreen(this);
    current = screen;
    screen->add(element);
    push_back(screen);
  }
  if(element->opening(YARS_STRING_RECORDING))
  {
    current = _recording;
    _recording->add(element);
  }

  if(element->closing(YARS_STRING_SCREEN_CONFIG))
  {
    current = parent;
  }

  if(element->opening(YARS_STRING_CAMERA_CONFIG))
  {
    current = _cameraConfig;
    current->add(element);
  }

  if(element->opening(YARS_STRING_FOLLOWABLES))
  {
    _followables = new DataFollowables(this);
    current = _followables;
    _followables->add(element);
  }

  if(element->opening(YARS_STRING_JOINTS))
  {
    _visualiseJoints = true;
    element->set(YARS_STRING_HEIGHT, _jointsAxisDimension.height);
    element->set(YARS_STRING_RADIUS, _jointsAxisDimension.radius);
    element->set(YARS_STRING_X,      _xTexture);
    element->set(YARS_STRING_Y,      _yTexture);
    element->set(YARS_STRING_Z,      _zTexture);
  }

  if(element->opening(YARS_STRING_SKY)) _sky = element->attribute(YARS_STRING_NAME)->value();
}

void DataScreens::createXsd(XsdSpecification *spec)
{
  XsdSequence *screenConfigDefinition = new XsdSequence(YARS_STRING_SCREEN_CONFIG_DEFINITION);
  screenConfigDefinition->add(NE(YARS_STRING_SCREEN,        YARS_STRING_SCREEN_DEFINITION,        1));
  screenConfigDefinition->add(NE(YARS_STRING_DEFAULT,       YARS_STRING_SCREEN_DEFINITION,        0, 1));
  screenConfigDefinition->add(NE(YARS_STRING_SKY,           YARS_STRING_NAME_DEFINITION,          0, 1));
  screenConfigDefinition->add(NE(YARS_STRING_CAMERA_CONFIG, YARS_STRING_CAMERA_CONFIG_DEFINITION, 0, 1));
  screenConfigDefinition->add(NE(YARS_STRING_FOLLOWABLES,   YARS_STRING_FOLLOWABLES_DEFINITION,   0, 1));
  screenConfigDefinition->add(NE(YARS_STRING_RECORDING,     YARS_STRING_RECORDING_DEFINITION,     0, 1));
  screenConfigDefinition->add(NE(YARS_STRING_VISUALISE,     YARS_STRING_VISUALISE_DEFINITION,     0, 1));
  spec->add(screenConfigDefinition);

  XsdSequence *visualiseDefinition = new XsdSequence(YARS_STRING_VISUALISE_DEFINITION);
  visualiseDefinition->add(NE(YARS_STRING_JOINTS, YARS_STRING_JOINTS_AXIS_DEFINITION, 0, 1));
  spec->add(visualiseDefinition);

  XsdSequence *jointsVisDefinition = new XsdSequence(YARS_STRING_JOINTS_AXIS_DEFINITION);
  jointsVisDefinition->add(NA(YARS_STRING_HEIGHT, YARS_STRING_POSITIVE_DECIMAL, false));
  jointsVisDefinition->add(NA(YARS_STRING_RADIUS, YARS_STRING_POSITIVE_DECIMAL, false));
  jointsVisDefinition->add(NA(YARS_STRING_X,      YARS_STRING_XSD_STRING,       false));
  jointsVisDefinition->add(NA(YARS_STRING_Y,      YARS_STRING_XSD_STRING,       false));
  jointsVisDefinition->add(NA(YARS_STRING_Z,      YARS_STRING_XSD_STRING,       false));
  spec->add(jointsVisDefinition);

  DataScreen::createXsd(spec);
  DataFollowables::createXsd(spec);
  DataCameraConfig::createXsd(spec);
  DataRecording::createXsd(spec);
}


DataFollowables* DataScreens::followables()
{
  return _followables;
}

string DataScreens::sky()
{
  return _sky;
}

DataScreens* DataScreens::copy()
{
  DataScreens *copy = new DataScreens(NULL);
  for(std::vector<DataScreen*>::iterator i = begin(); i != end(); i++)
  {
    copy->push_back((*i)->copy());
  }
  copy->_cameraConfig        = _cameraConfig->copy();
  copy->_sky                 = _sky;
  copy->_jointsAxisDimension = _jointsAxisDimension;
  copy->_visualiseJoints     = _visualiseJoints;
  copy->_xTexture            = _xTexture;
  copy->_yTexture            = _yTexture;
  copy->_zTexture            = _zTexture;
  copy->_recording           = _recording->copy();

  if(_followables  != NULL) copy->_followables  = _followables->copy();
  return copy;
}

DataCameraConfig* DataScreens::cameraConfig()
{
  return _cameraConfig;
}

bool DataScreens::visualiseJoints()
{
  return _visualiseJoints;
}

RHDimension DataScreens::jointsVisualisationDimension()
{
  return _jointsAxisDimension;
}

string DataScreens::xAxisTexture()
{
  return _xTexture;
}

string DataScreens::yAxisTexture()
{
  return _yTexture;
}

string DataScreens::zAxisTexture()
{
  return _zTexture;
}

DataRecording* DataScreens::recording()
{
  return _recording;
}

bool DataScreens::hasVisible()
{
  bool r = false;
  for(std::vector<DataScreen*>::iterator i = begin(); i != end(); i++)
  {
    r |= (*i)->autoShow();
  }
  return r;
}
