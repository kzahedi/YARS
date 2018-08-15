#include "DataLoggingCSV.h"

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
    element->set(YARS_STRING_NAME,          _filename);
    element->set(YARS_STRING_DATE,          _useDate);
    element->set(YARS_STRING_START,         _start);
    element->set(YARS_STRING_STOP,          _stop);
    element->set(YARS_STRING_DEBUG,         _debug);
    element->set(YARS_STRING_USE_TIME_STEP, _useTimeStep);
  }
  if(element->opening(YARS_STRING_TARGET))
  {
    string target;
    element->set(YARS_STRING_NAME, target);
    _targets.push_back(target);
  }

}

void DataLoggingCSV::createXsd(XsdSpecification *spec)
{
  // TODO: define filename type
  XsdSequence *fileDefinition = new XsdSequence(YARS_STRING_CSV_LOGGER_DEFINITION);
  fileDefinition->add(NA(YARS_STRING_NAME,          YARS_STRING_XSD_STRING,                true));
  fileDefinition->add(NA(YARS_STRING_START,         YARS_STRING_POSITIVE_NON_ZERO_INTEGER, false));
  fileDefinition->add(NA(YARS_STRING_STOP,          YARS_STRING_POSITIVE_NON_ZERO_INTEGER, false));
  fileDefinition->add(NA(YARS_STRING_DATE,          YARS_STRING_TRUE_FALSE_DEFINITION,     false));
  fileDefinition->add(NA(YARS_STRING_DEBUG,         YARS_STRING_TRUE_FALSE_DEFINITION,     false));
  fileDefinition->add(NA(YARS_STRING_USE_TIME_STEP, YARS_STRING_TRUE_FALSE_DEFINITION,     false));
  fileDefinition->add(NE(YARS_STRING_TARGET,        YARS_STRING_TARGET_DEFINITION,         0));
  spec->add(fileDefinition);

  XsdSequence *targetDefinition = new XsdSequence(YARS_STRING_TARGET_DEFINITION);
  targetDefinition->add(NA(YARS_STRING_NAME,    YARS_STRING_XSD_STRING, true));
  spec->add(targetDefinition);

  XsdEnumeration *trueFalseDefinition = new XsdEnumeration(YARS_STRING_TRUE_FALSE_DEFINITION, YARS_STRING_XSD_STRING);
  trueFalseDefinition->add(YARS_STRING_TRUE);
  trueFalseDefinition->add(YARS_STRING_FALSE);
  spec->add(trueFalseDefinition);

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

