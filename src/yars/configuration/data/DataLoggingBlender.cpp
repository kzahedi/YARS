#include "DataLoggingBlender.h"
#include "DataBinding.h"

#include <cstdlib>

#define YARS_STRING_NAME                      (char*)"name"
#define YARS_STRING_CFG                       (char*)"cfg"
#define YARS_STRING_FRAME_RATE                (char*)"framerate"
#define YARS_STRING_POSITIVE_NON_ZERO_INTEGER (char*)"positive_non_zero_integer_definition"

namespace
{
// Attribute binding table for the blender logger's own opening tag.
// No child-element dispatch remains below.
const std::vector<yars::AttributeBinding> &loggingBlenderAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_NAME,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingBlender *>(self)->setFilename(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_CFG,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingBlender *>(self)->setCfg(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_FRAME_RATE,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingBlender *>(self)->setFramerate(atoi(value.c_str())); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

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
    yars::applyAttributes(this, element, loggingBlenderAttributeBindings());
  }
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

void DataLoggingBlender::setFilename(string filename)
{
  _filename = filename;
}

void DataLoggingBlender::setCfg(string cfg)
{
  _cfg = cfg;
}

void DataLoggingBlender::setFramerate(int framerate)
{
  _framerate = framerate;
}

string DataLoggingBlender::cfg()
{
  return _cfg;
}

int DataLoggingBlender::framerate()
{
  return _framerate;
}
