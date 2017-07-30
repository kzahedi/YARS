#ifndef __SELFORG_LOGGER_H__
#define __SELFORG_LOGGER_H__

#include <yars/logging/Logger.h>
#include <yars/configuration/data/DataLoggingSelforg.h>

#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

class SelforgLogger : public Logger
{
  public:
    SelforgLogger(DataLoggingSelforg *data, int index);
    virtual ~SelforgLogger() { };

    void update();
    void init();
    void close();

  private:
    string __convert(string in);
    FILE               *_guiloggerFD;
    FILE               *_matrixvizFD;
    DataLoggingSelforg *_data;
    stringstream        _oss;
    int                 _fileIndex;
    std::vector<string> _labels;
    ofstream            _output;
    bool                _useFile;
    bool                _useMatrixviz;
    bool                _useGuilogger;
    string              _filename;
    double               _time;
    double               _dtime;
    int                 _matrixvizStepSize;
    int                 _guiloggerStepSize;
    int                 _frequency;
};

#endif // __SELFORG_LOGGER_H__
