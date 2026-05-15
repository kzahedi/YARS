#ifndef __GNUPLOT_LOGGER_H__
#define __GNUPLOT_LOGGER_H__

#include <yars/logging/Logger.h>
#include <yars/configuration/data/DataLoggingGnuplot.h>
#include <yars/util/CircularBuffer.h>

#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

class GnuplotLogger : public Logger
{
  public:
    GnuplotLogger(DataLoggingGnuplot *data, int index);

    void update();
    void init();
    void close();

  private:
    FILE               *_gnuplotFD;
    DataLoggingGnuplot *_data;
    stringstream        _oss;
    int                 _fileIndex;
    std::vector<string>      _labels;
    //string              _dataFilename;
    //ofstream            _dataFile;
    bool                _firstCall;
    unsigned long       _printTimeLastMeasurement;
    unsigned long       _delay;
    bool                _pairwise;
    string              _plotString;

    std::vector<CircularBuffer<string> > _buffer;
};

#endif // __GNUPLOT_LOGGER_H__


