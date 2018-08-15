#ifndef __DATA_SIGNAL_KEYBOARD_H__
#define __DATA_SIGNAL_KEYBOARD_H__

#include "DataSignal.h"
#include "DataParameter.h"
#include <vector>
#include <string>

# define YARS_STRING_KEYBOARD_SIGNAL            (char*)"keyboard"
# define YARS_STRING_KEYBOARD_SIGNAL_DEFINITION (char*)"keyboard_definition"

using namespace std;

class DataSignalKeyboard : public DataSignal, public std::vector<DataParameter*>
{
  public:
    DataSignalKeyboard(DataNode *parent);

    void add(DataParseElement *element);
    string name();
    string module();
    string key();

    static void createXsd(XsdSpecification *spec);

    DataSignalKeyboard* _copy();

  private:
    string                 _name;
    string                 _module;
    string                 _key;

};

#endif // __DATA_SIGNAL_KEYBOARD_H__
