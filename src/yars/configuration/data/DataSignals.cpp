#include "DataSignals.h"
#include "DataSignalKeyboard.h"
#include "DataSignalPeriodic.h"
#include "DataSignalTriggered.h"

DataSignals::DataSignals(DataNode *parent)
  : DataNode(parent)
{

}

void DataSignals::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_SIGNALS_LIST))
  {
    current = parent;
  }
  if(element->opening(YARS_STRING_KEYBOARD_SIGNAL))
  {
    DataSignalKeyboard *signal = new DataSignalKeyboard(this);
    push_back(signal);
    current = signal;
    signal->add(element);
  }
  if(element->opening(YARS_STRING_PERIODIC_SIGNAL))
  {
    DataSignalPeriodic *signal = new DataSignalPeriodic(this);
    push_back(signal);
    current = signal;
    signal->add(element);
  }
  if(element->opening(YARS_STRING_TRIGGERED_SIGNAL))
  {
    DataSignalTriggered *signal = new DataSignalTriggered(this);
    push_back(signal);
    current = signal;
    signal->add(element);
  }
}

void DataSignals::createXsd(XsdSpecification *spec)
{
  XsdSequence *signalListDefinition = new XsdSequence(YARS_STRING_SIGNALS_LIST_DEFINITION);
  signalListDefinition->add(NE(YARS_STRING_KEYBOARD_SIGNAL,  YARS_STRING_KEYBOARD_SIGNAL_DEFINITION,  0));
  signalListDefinition->add(NE(YARS_STRING_PERIODIC_SIGNAL,  YARS_STRING_PERIODIC_SIGNAL_DEFINITION,  0));
  signalListDefinition->add(NE(YARS_STRING_TRIGGERED_SIGNAL, YARS_STRING_TRIGGERED_SIGNAL_DEFINITION, 0));
  spec->add(signalListDefinition);

  DataSignalKeyboard::createXsd(spec);
  DataSignalPeriodic::createXsd(spec);
  DataSignalTriggered::createXsd(spec);
}

DataSignals* DataSignals::copy()
{
  DataSignals* copy = new DataSignals(NULL);
  for(std::vector<DataSignal*>::iterator i = begin(); i != end(); i++)
  {
    copy->push_back((*i)->copy());
  }
  return copy;
}
