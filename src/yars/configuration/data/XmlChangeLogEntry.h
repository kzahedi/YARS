#ifndef __XML_CHANGE_LOG_ENTRY_H__
#define __XML_CHANGE_LOG_ENTRY_H__

#include <yars/types/Version.h>

#include <yars/defines/types.h>

#include <string>

using namespace std;

class XmlChangeLogEntry
{
  public:
    XmlChangeLogEntry(Version version, string description, bool crucial);
    Version version();
    bool crucial();
    string description();

  private:

    Version _version;
    string _description;
    bool   _crucial;

};


#endif // __XML_CHANGE_LOG_ENTRY_H__
