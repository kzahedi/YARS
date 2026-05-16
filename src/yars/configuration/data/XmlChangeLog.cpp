#include <yars/configuration/data/XmlChangeLog.h>


#include <sstream>

XmlChangeLog* XmlChangeLog::_me = NULL;

XmlChangeLog::XmlChangeLog()
{ }

void XmlChangeLog::initialize()
{
  // The change-log entries live in yars/defines/version.h as a bare
  // sequence of XmlChangeLog::add(...) statements. The file is meant to
  // be #included at function scope; using a static guard makes the call
  // idempotent so we can invoke initialize() from both the CLI path
  // (so `yars --version` reports a real version) and from XML parsing
  // (preserves the original DataRobotSimulationDescription contract)
  // without piling up duplicate entries.
  static bool _populated = false;
  if (_populated) return;
  _populated = true;
#include <yars/defines/version.h>
}

XmlChangeLog::~XmlChangeLog()
{
  close();
}

void XmlChangeLog::close()
{
  if (_me != NULL)
  {
    for (auto *x : *_me) delete x;
    delete _me;
  }
}

void XmlChangeLog::add(Version version, string description, bool crucial)
{
  if(_me == NULL) _me = new XmlChangeLog();
  XmlChangeLogEntry *entry = new XmlChangeLogEntry(version, description, crucial);
  _me->push_back(entry);
  sort(_me->begin(), _me->end(), XmlChangeLog::compare);
  _me->_version = (*_me)[_me->size()-1]->version();
  for(int i = _me->size()-1; i >= 0; i--)
  {
    if(_me->at(i)->crucial())
    {
      _me->_last_crucial_change = _me->at(i)->version();
      break;
    }
  }
}

void XmlChangeLog::add(int major, int minor, int patch, string description, bool crucial)
{
  if(_me == NULL) _me = new XmlChangeLog();
  Version v(major, minor, patch);
  add(v, description, crucial);
}

Version XmlChangeLog::version()
{
  if(_me == NULL) _me = new XmlChangeLog();
  return _me->_version;
}


Version XmlChangeLog::last_crucial_change()
{
  if(_me == NULL) _me = new XmlChangeLog();
  return _me->_last_crucial_change;
}

string XmlChangeLog::changes(Version version)
{
  if(_me == NULL) _me = new XmlChangeLog();
  stringstream oss;
  int size = 0;
  for (auto *x : *_me)
  {
    oss.str("");
    oss.precision(1);
    oss.setf(ios::fixed, ios::floatfield);
    oss << x->version();
    if ((int)oss.str().length() > size) size = oss.str().length();
  }
  oss.str("");
  for (auto *x : *_me)
  {
    if (x->version() > version)
    {
      oss.width(size);
      oss << x->version();
      oss.width(0);
      oss << " -- ";
      if (x->crucial()) oss << "crucial ";
      else              oss << "optional";
      oss << " -- " << x->description() << endl;
    }
  }
  return oss.str();
}

bool XmlChangeLog::compare(XmlChangeLogEntry *a, XmlChangeLogEntry *b)
{
  return a->version() < b->version();
}
