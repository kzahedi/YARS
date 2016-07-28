#ifndef __XML_CHANGE_LOG_H__
#define __XML_CHANGE_LOG_H__

#include <yars/configuration/data/XmlChangeLogEntry.h>

#include <yars/types/Version.h>

#include <yars/defines/types.h>

#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class XmlChangeLog : public std::vector<XmlChangeLogEntry*>
{
  public:
    ~XmlChangeLog();

    static void add(Version version, string description, bool crucial = false);
    static void add(int major, int minor, int patch, string description, bool crucial = false);
    static Version version();
    static Version last_crucial_change();

    static void close();
    static string changes(Version version);

    static bool compare(XmlChangeLogEntry *a, XmlChangeLogEntry *b);

  private:
    XmlChangeLog();
    Version _version; // last version
    Version _last_crucial_change;

    static XmlChangeLog *_me;

};

#endif // __XML_CHANGE_LOG_H__
