#ifndef __VERSION_H__
#define __VERSION_H__

#include <iostream>

class Version
{
  public:
    Version();
    Version(const Version &v); // copy constructor
    Version(int major, int minor, int patch);

    int major;
    int minor;
    int patch;

    bool operator<(const Version &v) const;
    bool operator>(const Version &v) const;
    bool operator==(const Version &v) const;
    Version & operator=(const Version &b);

    friend std::ostream& operator<<(std::ostream& str, const Version& v)
    {
      str << v.major << "." << v.minor << "." << v.patch;
      return str;
    };

};

#endif // __VERSION_H__
