#ifndef __BLENDER_LOGGER_H__
#define __BLENDER_LOGGER_H__

#include <yars/logging/Logger.h>
#include <yars/configuration/data/DataLoggingBlender.h>

#include <iostream>
#include <fstream>

class BlenderLogger : public Logger
{
  public:
    BlenderLogger(DataLoggingBlender *data);

    void update();
    void init();
    void close();

  private:
    DataLoggingBlender *_data;
    stringstream        _oss;
    ofstream            _output;
    int                 _step;
};

#endif // __BLENDER_LOGGER_H__
