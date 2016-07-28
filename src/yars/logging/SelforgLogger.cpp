#include <yars/logging/SelforgLogger.h>

#include <yars/util/Timer.h>
#include <yars/util/stl_macros.h>
#include <yars/configuration/YarsConfiguration.h>

#ifdef _MSC_VER
#  define popen _popen
#  define pclose _pclose
#endif

SelforgLogger::SelforgLogger(DataLoggingSelforg *data, int index)
{
  _data              = data;
  // _frequency         = data->frequency();
  _fileIndex         = index;
  _useFile           = data->useFile();
  _useMatrixviz      = data->useMatrixviz();
  _useGuilogger      = data->useGuilogger();
  _filename          = data->filename();
  _dtime             = __YARS_GET_STEP_SIZE * _frequency;
  _matrixvizFD       = NULL;
  _guiloggerFD       = NULL;
  _guiloggerStepSize = data->glStepSize();
  _matrixvizStepSize = data->mvStepSize();

  if(_useFile)
  {
    string dateTimeString;
    Timer::getDateTimeString(&dateTimeString);
    _oss << _data->filename() << "-" << dateTimeString << ".txt";
    _output.open(_oss.str().c_str());
  }

  if(_useMatrixviz) _matrixvizFD = popen("matrixviz","w");
  if(_useGuilogger) _guiloggerFD = popen("guilogger -m pipe","w");

  for(std::vector<string>::iterator s = data->t_begin(); s != data->t_end(); s++)
  {
    push_back(*s);
  }
}

void SelforgLogger::init()
{
  _time = 0;
  std::vector<string> names;
  names.push_back("t");
  for(std::vector<LoggingModule*>::iterator l = _modules.begin(); l != _modules.end(); l++)
  {
    for(std::vector<string>::iterator v = (*l)->v_begin(); v != (*l)->v_end(); v++)
    {
      names.push_back(*v);
    }
  }

  _oss.str("");
  _oss << "#C";
  FOREACH(string, n, names)
  {
    _oss << " " << __convert(*n);
  }
  _oss << endl;

  string line = _oss.str();

  if(_useFile)      _output << line;
  if(_useMatrixviz) fprintf(_matrixvizFD,"%s",line.c_str());
  if(_useGuilogger) fprintf(_guiloggerFD,"%s",line.c_str());

}

string SelforgLogger::__convert(string in)
{
  for(int i = 0; i < (int)in.length(); i++)
  {
    if(in[i] == ' ') in[i] = '_';
  }
  return in;
}

void SelforgLogger::update()
{
  _time = ((yReal)__YARS_GET_STEP) * _dtime;
  std::vector<string> values;
  _oss.str("");
  for(std::vector<LoggingModule*>::iterator l = _modules.begin(); l != _modules.end(); l++)
  {
    for(std::vector<string>::iterator v = (*l)->begin(); v != (*l)->end(); v++)
    {
      values.push_back(*v);
    }
  }
  _oss << _time;
  if(values.size() > 0)
  {
    _oss << " ";
    for(int i = 0; i < (int)values.size() - 1; i++)
    {
      _oss << values[i] << " ";
    }
    _oss << values[values.size() - 1];
  }
  _oss << endl;

  string line = _oss.str();

  if(_useFile)
  {
    _output << line;
  }

  if(_useMatrixviz && (__YARS_GET_STEP % _matrixvizStepSize == 0)) fprintf(_matrixvizFD,"%s",line.c_str());
  if(_useGuilogger && (__YARS_GET_STEP % _guiloggerStepSize == 0)) fprintf(_guiloggerFD,"%s",line.c_str());
}

void SelforgLogger::close()
{
  if(_matrixvizFD != NULL)
  {
    fprintf(_matrixvizFD,"#QUIT\n");
    fclose(_matrixvizFD);
  }
  if(_guiloggerFD != NULL)
  {
    fprintf(_guiloggerFD,"#QUIT\n");
    fclose(_guiloggerFD);
  }
}
