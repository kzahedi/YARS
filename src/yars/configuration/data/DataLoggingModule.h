#ifndef __DATA_LOGGING_MODULE_INTERFACE_H__
#define __DATA_LOGGING_MODULE_INTERFACE_H__

#include <string>
#include <vector>

using namespace std;

class DataLoggingModule
{
  public:

    DataLoggingModule();
    ~DataLoggingModule();

    std::vector<string>::iterator t_begin();
    std::vector<string>::iterator t_end();
    int                      t_size();

    string                   delimiter();

  protected:
    std::vector<string>  _targets;
    string          _delimiter;

};

#endif // __DATA_LOGGING_MODULE_INTERFACE_H__
