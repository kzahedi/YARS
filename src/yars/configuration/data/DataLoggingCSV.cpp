#include "DataLoggingCSV.h"
#include "DataBinding.h"

#include <cstdlib>

#define YARS_STRING_TARGET                (char*)"target"
#define YARS_STRING_MODULE                (char*)"module"
#define YARS_STRING_NAME                  (char*)"name"

#define YARS_STRING_DEBUG                 (char*)"debug"
#define YARS_STRING_START                 (char*)"start"
#define YARS_STRING_STOP                  (char*)"stop"
#define YARS_STRING_USE_TIME_STEP         (char*)"usetimestep"

#define YARS_STRING_DATE                  (char*)"date"
#define YARS_STRING_TRUE                  (char*)"true"
#define YARS_STRING_FALSE                 (char*)"false"

#define YARS_STRING_TARGET                (char*)"target"
#define YARS_STRING_TARGET_DEFINITION     (char*)"target_definition"

#define YARS_STRING_TRUE_FALSE_DEFINITION (char*)"true_false_definition"


namespace
{
// Attribute binding table for the csv logger's own opening tag.
// Child-element dispatch (target) stays hand-written below.
const std::vector<yars::AttributeBinding> &loggingCSVAttributeBindings()
{
  static const std::vector<yars::AttributeBinding> bindings = {
      {YARS_STRING_NAME,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingCSV *>(self)->setFilename(value); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_DATE,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingCSV *>(self)->setUseDate(value == "true"); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_START,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingCSV *>(self)->setStart(atoi(value.c_str())); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_STOP,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingCSV *>(self)->setStop(atoi(value.c_str())); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_DEBUG,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingCSV *>(self)->setDebug(value == "true"); },
       /*required=*/false, /*defaultValue=*/nullptr},
      {YARS_STRING_USE_TIME_STEP,
       [](DataNode *self, const std::string &value)
       { static_cast<DataLoggingCSV *>(self)->setUseTimeStep(value == "true"); },
       /*required=*/false, /*defaultValue=*/nullptr},
  };
  return bindings;
}
} // namespace

DataLoggingCSV::DataLoggingCSV(DataNode *parent)
  : DataNode(parent)
{
  _useDate     = true;
  _start       = 0;
  _stop        = -1;
  _useTimeStep = false;
  _debug       = false;
}

DataLoggingCSV::~DataLoggingCSV()
{
  // nothing to be done
}

void DataLoggingCSV::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_CSV_LOGGER))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_CSV_LOGGER))
  {
    yars::applyAttributes(this, element, loggingCSVAttributeBindings());
  }
  if(element->opening(YARS_STRING_TARGET))
  {
    string target;
    element->set(YARS_STRING_NAME, target);
    _targets.push_back(target);
  }

}

DataLoggingCSV* DataLoggingCSV::copy()
{
  DataLoggingCSV *copy = new DataLoggingCSV(NULL);
  for(std::vector<string>::iterator i = _targets.begin(); i != _targets.end(); i++)
  {
    copy->_targets.push_back(*i);
  }
  copy->_filename    = _filename;
  copy->_useDate     = _useDate;
  copy->_useTimeStep = _useTimeStep;
  copy->_start       = _start;
  copy->_stop        = _stop;
  copy->_debug       = _debug;
  return copy;
}

string DataLoggingCSV::filename()
{
  return _filename;
}

void DataLoggingCSV::setFilename(string filename)
{
  _filename = filename;
}

void DataLoggingCSV::setUseDate(bool useDate)
{
  _useDate = useDate;
}

void DataLoggingCSV::setStart(int start)
{
  _start = start;
}

void DataLoggingCSV::setStop(int stop)
{
  _stop = stop;
}

void DataLoggingCSV::setDebug(bool debug)
{
  _debug = debug;
}

void DataLoggingCSV::setUseTimeStep(bool useTimeStep)
{
  _useTimeStep = useTimeStep;
}

bool DataLoggingCSV::useDate()
{
  return _useDate;
}

bool DataLoggingCSV::useTimeStep()
{
  return _useTimeStep;
}

bool DataLoggingCSV::isLogging(unsigned int timestep)
{
  if(_stop > 0)
  {
    if(timestep >= _start && timestep <= _stop) return true;
  }
  else
  {
    if(timestep > _start) return true;
  }
  return false;
}

bool DataLoggingCSV::debug()
{
  return _debug;
}

