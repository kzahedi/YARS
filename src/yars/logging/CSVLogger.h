#ifndef __CSV_LOGGER_H__
#define __CSV_LOGGER_H__

#include <yars/logging/Logger.h>
#include <yars/configuration/data/DataLoggingCSV.h>

#include <iostream>
#include <fstream>

using namespace std;

class CSVLogger : public Logger
{
  public:
    CSVLogger(DataLoggingCSV *data);

    void update() override;
    void init() override;
    void close() override;

  private:

    DataLoggingCSV *_data;
    stringstream    _oss;
    ofstream        _output;

};

#endif // __CSV_LOGGER_H__

