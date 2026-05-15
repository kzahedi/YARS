#ifndef __ROBOT_CONTROLLER_PARAMETER_H__
#define __ROBOT_CONTROLLER_PARAMETER_H__

#include <map>
#include <string>
#include <string.h>
#include <vector>
#include <cstdlib>
#include <iostream>

using namespace std;

class RobotControllerParameter : public map<string,string>
{
  public:
    bool exists(const string &key)
    {
      return find(key) != end();
    };

    void add(string name, string value)
    {
      insert(std::make_pair(name, value));
    }

    void get(string name, string *value)
    {
      if (auto iter = find(name); iter != end())
        *value = iter->second;
      else
        *value = "";
    }

    int intValue(string name)
    {
      string s;
      get(name, &s);
      return atoi(s.c_str());
    }

    vector<int> intList(string name)
    {
      vector<int> lst;
      string s;
      get(name, &s);
      char *str = (char*)s.c_str();
      char *pch = strtok (str," ,.-");
      while (pch != NULL)
      {
        lst.push_back(atoi(pch));
        pch = strtok (NULL, " ,.-");
      }
      return lst;
    }

    vector<double> doubleList(string name)
    {
      vector<double> lst;
      string s;
      get(name, &s);
      char *str = (char*)s.c_str();
      char *pch = strtok (str," ,.-");
      while (pch != NULL)
      {
        lst.push_back(atof(pch));
        pch = strtok (NULL, " ,.-");
      }
      return lst;
    }

    float floatValue(string name)
    {
      string s;
      get(name, &s);
      return atof(s.c_str());
    }

    double doubleValue(string name)
    {
      string s;
      get(name, &s);
      return (double)atof(s.c_str());
    }

    string stringValue(string name)
    {
      string s;
      get(name, &s);
      return s;
    }

    bool boolValue(string name)
    {
      string s;
      get(name, &s);
      return (s == "true");
    }

    std::vector<string> names()
    {
      std::vector<string> names;
      for (const auto &[key, value] : *this)
      {
        (void)value;
        names.push_back(key);
      }
      return names;
    }

    void set(string name, double &value, double def)
    {
      if(exists(name)) value = doubleValue(name);
      else             value = def;
    }

    void set(string name, float &value, float def)
    {
      if(exists(name)) value = floatValue(name);
      else             value = def;
    }

    void set(string name, bool &value, bool def)
    {
      if(exists(name)) value = boolValue(name);
      else             value = def;
    }

    void set(string name, int &value, int def)
    {
      if(exists(name)) value = intValue(name);
      else             value = def;
    }

    void set(string name, string &value, string def)
    {
      if(exists(name)) value = stringValue(name);
      else             value = def;
    }

    friend std::ostream& operator<<(std::ostream& str, const RobotControllerParameter& p)
    {
      str << "Robot Controller Parameter given: " << endl;
      for (const auto &[key, value] : p)
      {
        str << "  " << key << " = " << value << endl;
      }
      return str;
    };


};

#endif // __ROBOT_CONTROLLER_PARAMETER_H__


