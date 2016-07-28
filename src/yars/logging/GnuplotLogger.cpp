#include <yars/logging/GnuplotLogger.h>

#include <yars/util/Timer.h>
#include <yars/util/stl_macros.h>
#include <yars/util/YarsErrorHandler.h>

#ifdef _MSC_VER
#  define popen _popen
#  define pclose _pclose
#endif

GnuplotLogger::GnuplotLogger(DataLoggingGnuplot *data, int index)
{
  _data      = data;
  _delay = data->delay();
  _fileIndex = index;
  _oss.str("");
  _oss << "gnuplot-tmp-" << _fileIndex << ".txt";
  _firstCall = true;
  //_dataFilename = _oss.str();
  _pairwise = data->pairwise();

  for(std::vector<string>::iterator s = data->t_begin(); s != data->t_end(); s++)
  {
    push_back(*s);
  }
}

void GnuplotLogger::init()
{
  _gnuplotFD = popen("gnuplot","w");
  if(_gnuplotFD == NULL) YarsErrorHandler::push("Cannot open gnuplot executable");
  for(std::vector<LoggingModule*>::iterator l = _modules.begin(); l != _modules.end(); l++)
  {
    for(std::vector<string>::iterator v = (*l)->v_begin(); v != (*l)->v_end(); v++)
    {
      _labels.push_back(*v);
    }
  }
  _buffer.resize(_labels.size());
  for(int i = 0; i < (int)_labels.size(); i++)
  {
    _buffer[i].set_capacity(_data->size());
  }
  _printTimeLastMeasurement = -_delay;
}

void GnuplotLogger::update()
{
  std::vector<string> values;
  _oss.str("");
  for(std::vector<LoggingModule*>::iterator l = _modules.begin(); l != _modules.end(); l++)
  {
    for(std::vector<string>::iterator v = (*l)->begin(); v != (*l)->end(); v++)
    {
      values.push_back(*v);
    }
  }
  if(values.size() > 0)
  {
    for(int i = 0; i < (int)values.size(); i++)
    {
      _buffer[i].push_back(values[i]);
    }
  }

  if( (Timer::getTime() - _printTimeLastMeasurement) > _delay)
  {
    //_dataFile.open(_dataFilename.c_str());
    _oss.str();
    for(int i = 0; i < (int)_buffer.size(); i++)
    {
      //_dataFile << _buffer[i];
    }
    //_dataFile.close();
    _oss.str("");
    if(_firstCall)
    {
      if(_data->name().size() > 0)
      {
        _oss << "set term " << _data->term() << " title '" << _data->name() << "'" << endl;
      }
      else
      {
        _oss << "set term " << _data->term() << endl;
      }
      fprintf(_gnuplotFD, "%s", _oss.str().c_str());
      _firstCall = false;
    }
    if(_pairwise)
    {
      _oss << "plot '-' using 1:2 title \"" << _labels[0] << "," << _labels[1] << "\" with lines lw 2 ";
      for(int i = 1; i < (int)_labels.size()-2; i+=2)
      {
        _oss << ", '-' using 1:2 title \"" << _labels[i+1] << ", " << _labels[i+2] << "\" with lines lw 2 ";
      }
      _oss << endl;
      for(int i = 0; i < (int)_buffer.size(); i+=2)
      {
        for(int j = 0; j < (int)_buffer[i].size(); j++)
        {
          _oss << _buffer[i][j] << " " << _buffer[i+1][j] << endl;
        }
        _oss << "e" << endl;
      }
    }
    else
    {
      _oss << "plot '-' title \"" << _labels[0] << "\" with lines lw 2 ";
      for(int i = 1; i < (int)_labels.size(); i++)
      {
        _oss << ", '-' title \"" << _labels[i] << "\" with lines lw 2 ";
      }
      _oss << endl;
      for(int i = 0; i < (int)_buffer.size(); i++)
      {
        for(int j = 0; j < (int)_buffer[i].size(); j++)
        {
          _oss << _buffer[i][j] << endl;
        }
        _oss << "e" << endl;
      }
    }
    fprintf(_gnuplotFD, "%s", _oss.str().c_str());
    _printTimeLastMeasurement = Timer::getTime();
  }
}

void GnuplotLogger::close()
{
  fclose(_gnuplotFD);
  //remove(_dataFilename.c_str());
}
