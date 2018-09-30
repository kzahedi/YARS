#include <yars/logging/BlenderLogger.h>

#include <yars/configuration/data/Data.h>
#include <yars/configuration/YarsConfiguration.h>
#include <yars/util/YarsErrorHandler.h>
#include <yars/view/console/ConsoleView.h>

BlenderLogger::BlenderLogger(DataLoggingBlender *data)
{
  _data = data;
  if (_data->framerate() > 0)
  {
    if (__YARS_GET_SIMULATOR_FREQUENCY % _data->framerate() != 0)
    {
      YarsErrorHandler::push("Simulator frequency must be a multiple of the blender frame rate.");
    }
    _step = __YARS_GET_SIMULATOR_FREQUENCY / _data->framerate();
  }
  else
  {
    _step = 1;
  }
}

void BlenderLogger::update()
{
  if (__YARS_GET_STEP % _step == 0 || __YARS_GET_STEP == 1)
  {
    _oss.str("");
    _oss << "Frame," << __YARS_GET_STEP << ",";
    DataRobots *robots = Data::instance()->current()->robots();
    for (DataRobots::iterator r = robots->begin(); r != robots->end(); r++)
    {
      string name = (*r)->name();
      for (DataObjects::iterator o = (*r)->o_begin(); o != (*r)->o_end(); o++)
      {
        Pose p = (*o)->pose();
        _oss << name << " " << (*o)->name() << ","
             << p.position.x << "," << p.position.y << "," << p.position.z << ","
             << p.orientation.x << "," << p.orientation.y << "," << p.orientation.z << ",";
      }
    }
    string s = _oss.str();
    _output << s.substr(0, s.size() - 1) << endl;
  }
}

void BlenderLogger::init()
{
  _oss.str("");
  _oss << "opening " << _data->filename();
  Y_INFO(_oss.str());

  _output.open(_data->filename().c_str());
  if (_data->cfg() != "")
  {
    _oss.str("");
    _oss << "writing cfg file " << _data->cfg();
    Y_INFO(_oss.str());
    ofstream names;
    names.open(_data->cfg().c_str());
    names << "Frame Index" << endl;
    DataRobots *robots = Data::instance()->current()->robots();
    for (DataRobots::iterator r = robots->begin(); r != robots->end(); r++)
    {
      string name = (*r)->name();
      for (DataObjects::iterator o = (*r)->o_begin(); o != (*r)->o_end(); o++)
      {
        _oss.str("");
        _oss << name << " " << (*o)->name() << endl;
        names << _oss.str();
      }
    }
    names.close();
  }
}

void BlenderLogger::close()
{
  _output.close();
}
