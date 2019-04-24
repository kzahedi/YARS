#include "DataLoggingConsole.h"

#define YARS_STRING_TARGET            (char*)"target"
#define YARS_STRING_MODULE            (char*)"module"
#define YARS_STRING_NAME              (char*)"name"

#define YARS_STRING_TARGET            (char*)"target"
#define YARS_STRING_TARGET_DEFINITION (char*)"target_definition"


DataLoggingConsole::DataLoggingConsole(DataNode *parent)
  : DataNode(parent)
{
}

DataLoggingConsole::~DataLoggingConsole()
{
  // nothing to be done
}

void DataLoggingConsole::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_CONSOLE))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_CONSOLE))
  {
  }
  if(element->opening(YARS_STRING_TARGET))
  {
    string target;
    element->set(YARS_STRING_NAME, target);
    _targets.push_back(target);
  }
}

void DataLoggingConsole::createXsd(XsdSpecification *spec)
{
  // TODO: define filename type
  XsdSequence *consoleDefinition = new XsdSequence(YARS_STRING_CONSOLE_DEFINITION);
  consoleDefinition->add(XE(YARS_STRING_TARGET, YARS_STRING_TARGET_DEFINITION, 0));
  spec->add(consoleDefinition);

  XsdSequence *targetDefinition = new XsdSequence(YARS_STRING_TARGET_DEFINITION);
  targetDefinition->add(NA(YARS_STRING_NAME,       YARS_STRING_XSD_STRING, true));
  spec->add(targetDefinition);
}


DataLoggingConsole* DataLoggingConsole::copy()
{
  DataLoggingConsole *copy = new DataLoggingConsole(NULL);
  for(std::vector<string>::iterator i = _targets.begin(); i != _targets.end(); i++)
  {
    copy->_targets.push_back(*i);
  }
  return copy;
}
