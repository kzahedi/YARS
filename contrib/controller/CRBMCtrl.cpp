#include "CRBMCtrl.h"

#include <crbm/Random.h>
#include <entropy++/Csv.h>

#include <math.h>
#include <stdio.h>

using namespace std;

void CRBMCtrl::update()
{
  if(record == false)
  {
    for(int i = 0; i < (int)sensors.size(); i++) S[i] = sensors[i];
    _crbm->update(S, A);
    for(int i = 0; i < (int)motors.size();   i++) motors[i] = A[i];
    // for(int i = 0; i < (int)motors.size()-1; i++) cout << motors[i] << ",";
    // cout << motors[motors.size()-1] << endl;

    if(t < T)
    {
      vector<double> tmp = Adata->row(t);
      for(int i = 0; i < (int)motors.size(); i++) motors[i] = tmp[i];
    }
  }
  else
  {
    if(t >= Adata->rows()) exit(0);
    vector<double> tmp = Adata->row(t);
    for(int i = 0; i < (int)motors.size(); i++) motors[i] = tmp[i];
  }
  t++;
}

void CRBMCtrl::init()
{
  string crbm_filename;
  string a_filename;
  parameter.set("crbm",   crbm_filename, "");
  parameter.set("A",      a_filename,    "");
  parameter.set("init",   T,             100);
  parameter.set("record", record,        false);

  _crbm = new libcrbm::binary::CRBMController(crbm_filename);
  Adata = entropy::Csv::read(a_filename);

  S = new double[sensors.size()];
  A = new double[motors.size()];

  libcrbm::Random::initialise();

  t = 0;
}

void CRBMCtrl::reset()
{
  t = 0;
}

void CRBMCtrl::close()
{ }

// the class factories
extern "C" RobotController* create()
{
  CRBMCtrl* b = new CRBMCtrl();
  return (RobotController*)b;
}

extern "C" void destroy(RobotController* controller)
{
  //delete controller;
}
