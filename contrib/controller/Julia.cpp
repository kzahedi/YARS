#include "Julia.h"
#include <string>
#include <fstream>
#include <streambuf>
#include <julia.h>

using namespace std;

void Julia::update()
{
  size_t n    = sensors.size();
  size_t m    = motors.size();
  size_t l    = 256;
  char   str[l];

  int status = _update(&n, &sensors[0], &m, &motors[0], &l, &str[0]);

  clearOsd();
  addLine(str);

  if(_debug)
  {
    _oss.str("");
    _oss << sensors[0];
    for(size_t i = 1; i < sensors.size(); i++) _oss << " " << sensors[i];
    for(size_t i = 0; i < motors.size();  i++) _oss << " " << motors[i];
    _oss << endl;

    _log << _oss.str();
  }

  switch(status)
  {
    case 1:
      setReset();
      break;
    case 2:
      setQuit();
      break;
  }

}

void Julia::reset()
{ }

void Julia::init()
{
  string filename;
  string juliaDir;
  string logFileName;
  parameter.set("debug",         _debug,       false);
  parameter.set("file name",      filename,    "");
  parameter.set("log file name",  logFileName, "julia-controller.log");
  parameter.set("julia dir",      juliaDir,    ((string)JULIA_DIR) + ((string)"/bin"));

  if(filename == "")
  {
    cout << "Please give a julia file name." << endl;
    exit(-1);
  }

  if(_debug) cout << "Julia dir    " << juliaDir << endl;
  if(_debug) cout << "Reading file " << filename << endl;

  ifstream t(filename.c_str());
  string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());

  if(_debug) _log.open(logFileName.c_str());

  if(_debug) cout << "Initialising julia." << endl;

  jl_init((char*)juliaDir.c_str());
  if(_debug) cout << "Initialising stack base." << endl;
  // JL_SET_STACK_BASE;
  if(_debug) cout << "Reading julia file." << endl;
  void *ans = jl_eval_string(&str[0]);
  if(_debug) cout << "Done reading julia file." << endl;

  if (NULL == ans) {
    std::cerr << "Julia controller: error parsing julia source file" << std::endl;
    exit (1);
  }

  if(_debug) cout << "Getting update function." << endl;
  _update = (FuncPtr)jl_unbox_voidpointer((jl_value_t*)ans);

  int i = 1;
  stringstream oss;
  oss << "evaluation line " << i;
  vector<string> strs;
  while(parameter.exists(oss.str()))
  {
    strs.push_back(parameter.stringValue(oss.str()));
    i++;
    oss.str("");
    oss << "evaluation line " << i;
  }

  for(vector<string>::iterator s = strs.begin(); s != strs.end(); s++)
  {
    cout << *s << endl;
    ans = jl_eval_string((char*)(*s).c_str());
    if (NULL == ans) {
      std::cerr << "Julia controller: error parsing " << *s << std::endl;
      exit (1);
    }

  }
}

void Julia::close()
{ }

// the class factories

extern "C" RobotController* create() {return new Julia; }

extern "C" void destroy(RobotController* controller)
{
  // delete controller;
}
