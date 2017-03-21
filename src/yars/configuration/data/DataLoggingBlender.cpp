#include "DataLoggingBlender.h"

#define YARS_STRING_CFG                       (char*)"cfg"
#define YARS_STRING_FRAME_RATE                (char*)"framerate"
#define YARS_STRING_POSITIVE_NON_ZERO_INTEGER (char*)"positive_non_zero_integer" DIVIDER DEFINITION

DataLoggingBlender::DataLoggingBlender(DataNode *parent)
  : DataNode(parent)
{
  _framerate = 0;
}

DataLoggingBlender::~DataLoggingBlender()
{
  // nothing to be done
}

void DataLoggingBlender::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_BLENDER_LOGGER))
  {
    current = parent;
    return;
  }
  if(element->opening(YARS_STRING_BLENDER_LOGGER))
  {
    element->set(YARS_STRING_NAME,       _filename);
    element->set(YARS_STRING_CFG,        _cfg);
    element->set(YARS_STRING_FRAME_RATE, _framerate);
  }
}

void DataLoggingBlender::createXsd(XsdSpecification *spec)
{
  XsdSequence *blenderDefinition = new XsdSequence(YARS_STRING_BLENDER_LOGGER_DEFINITION);
  blenderDefinition->add(NA(YARS_STRING_NAME,       YARS_STRING_XSD_STRING,                true));
  blenderDefinition->add(NA(YARS_STRING_CFG,        YARS_STRING_XSD_STRING,                false));
  blenderDefinition->add(NA(YARS_STRING_FRAME_RATE, YARS_STRING_POSITIVE_NON_ZERO_INTEGER, false));
  spec->add(blenderDefinition);
}


DataLoggingBlender* DataLoggingBlender::copy()
{
  DataLoggingBlender *copy = new DataLoggingBlender(NULL);
  copy->_filename  = _filename;
  copy->_cfg       = _cfg;
  copy->_framerate = _framerate;
  return copy;
}

string DataLoggingBlender::filename()
{
  return _filename;
}

string DataLoggingBlender::cfg()
{
  return _cfg;
}

int DataLoggingBlender::framerate()
{
  return _framerate;
}
