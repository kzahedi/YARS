#include "NMODEController.h"

#include <nmode/RnnFromIndividual.h>

using namespace std;

void NMODEController::update()
{
  if(_debug)
  {
    cout << "Sensors: (" << sensors.size() << "):";
    for(vector<yReal>::iterator i = sensors.begin(); i != sensors.end(); i++)
    {
      cout << " " << *i;
    }
    cout << endl;
  }

  _rnn->setInputs(sensors);
  _rnn->update();
  _rnn->getOutput(motors);


  if(_debug)
  {
    cout << "Motors: (" << motors.size() << "):";
    for(vector<yReal>::iterator i = motors.begin(); i != motors.end(); i++)
    {
      cout << " " << *i;
    }
    cout << endl;
  }
}

void NMODEController::reset()
{ }

void NMODEController::init()
{
  parameter.set("debug",      _debug,      false);
  parameter.set("file name",  _filename,   "./last-generation.xml");
  parameter.set("individual", _individual, 0);


  _data = Data::instance();
  _data->read(_filename);

  Individual* individual = _data->specification()->population()->individual(_individual);
  _rnn = RnnFromIndividual::create(individual);
  cout << _rnn->nrOfSensors() << " " << _rnn->nrOfActuators() << " " << _rnn->nrOfNeurons() << " " << _rnn->nrOfSynapses() << endl;

}

void NMODEController::close()
{ }


// the class factories

extern "C" RobotController* create() {
  return new NMODEController;
}

extern "C" void destroy(RobotController* controller) {
  // delete controller;
}

