#ifndef __FILE_LOGGER_H__
#define __FILE_LOGGER_H__

#include <yars/logging/Logger.h>
#include <yars/configuration/data/DataLoggingFile.h>

#include <iostream>
#include <fstream>

using namespace std;

class FileLogger : public Logger
{
  public:
    FileLogger(DataLoggingFile *data);

    void update();
    void init();
    void close();

  private:

    DataLoggingFile *_data;
    stringstream     _oss;
    ofstream         _output;

};

#endif // __FILE_LOGGER_H__

