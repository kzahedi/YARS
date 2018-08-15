#include <yars/configuration/data/DataRecording.h>
#include <yars/configuration/data/DataActuators.h>

#include <yars/util/Directories.h>
#include <yars/util/YarsErrorHandler.h>

#include <yars/configuration/YarsConfiguration.h>

#include <yars/defines/defaults.h>

# define YARS_STRING_MODULE              (char*)"module"
# define YARS_STRING_NAME                (char*)"name"
# define YARS_STRING_FREQUENCY           (char*)"frequency"
# define YARS_STRING_POSITIVE_INTEGER    (char*)"positive_integer_definition"
# define YARS_STRING_START               (char*)"start"
# define YARS_STRING_END                 (char*)"end"
# define YARS_STRING_INTERVAL            (char*)"interval"
# define YARS_STRING_INTERVAL_DEFINITION (char*)"interval_definition"

DataRecording::DataRecording(DataNode *parent)
  : DataNode(parent)
{
}

void DataRecording::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_RECORDING))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_INTERVAL))
  {
    RecordingInterval interval;
    element->set(YARS_STRING_START, interval.min);
    element->set(YARS_STRING_END,   interval.max);
    push_back(interval);
  }
}

void DataRecording::createXsd(XsdSpecification *spec)
{
  XsdSequence *recording = new XsdSequence(YARS_STRING_RECORDING_DEFINITION);
  recording->add(NE(YARS_STRING_INTERVAL, YARS_STRING_INTERVAL_DEFINITION, 0));
  spec->add(recording);

  XsdSequence *intervalDefinition = new XsdSequence(YARS_STRING_INTERVAL_DEFINITION);
  intervalDefinition->add(NA(YARS_STRING_START, YARS_STRING_POSITIVE_INTEGER, true));
  intervalDefinition->add(NA(YARS_STRING_END,   YARS_STRING_POSITIVE_INTEGER, true));
  spec->add(intervalDefinition);
}

DataRecording* DataRecording::copy()
{
  DataRecording *copy = new DataRecording(NULL);
  for(std::vector<RecordingInterval>::iterator i = begin(); i != end(); i++)
  {
    copy->push_back(*i);
  }
  return copy;
}


void DataRecording::resetTo(const DataRecording*)
{
  YarsErrorHandler::push("DataRecording::resetTo not implemented");
  // clear();
  // for(std::vector<RecordingInterval>::iterator i = other->begin(); i != other->end(); i++)
  // {
    // push_back(*i);
  // }
}

bool DataRecording::record()
{
  if(size() == 0) return false;
  for(std::vector<RecordingInterval>::iterator i = begin(); i != end(); i++)
  {
    if((*i).min <= __YARS_GET_STEP && __YARS_GET_STEP <= (*i).max) return true;
  }
  return false;
}
