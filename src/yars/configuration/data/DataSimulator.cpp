#include <yars/configuration/data/DataSimulator.h>

#include <yars/defines/defaults.h>
#include <yars/util/YarsErrorHandler.h>

#include <iostream>

using namespace std;

# define YARS_STRING_CONTROL                   (char*)"control"
# define YARS_STRING_BEHAVIOUR                 (char*)"behaviour"
# define YARS_STRING_FREQUENCY                 (char*)"frequency"
# define YARS_STRING_AUTO_RESET                (char*)"reset"
# define YARS_STRING_SEED                      (char*)"seed"
# define YARS_STRING_SOLVER                    (char*)"solver"
# define YARS_STRING_POSITIVE_NON_ZERO_INTEGER (char*)"positive_non_zero_integer_definition"

# define YARS_STRING_COUNT                     (char*)"count"
# define YARS_STRING_COUNT_DEFINITION          (char*)"count_definition"


DataSimulator::DataSimulator(DataNode *parent)
  : DataNode(parent)
{
  _simulatorFrequency  = -1;
  _randomSeed          = -1;
  _useRandomSeed       = false;
  _iterations          = -1;
  _autoReset           = -1;
  _solverIterations    = 10;
  _controlType         = CONTROL_TYPE_SIMULATOR;
}

DataSimulator::~DataSimulator()
{
  // nothing to be done
}

int DataSimulator::simulatorFrequency()
{
  return _simulatorFrequency;
}

int DataSimulator::iterations()
{
  return _iterations;
}

void DataSimulator::setIterations(int iterations)
{
  _iterations = iterations;
}

int DataSimulator::autoReset()
{
  return _autoReset;
}

void DataSimulator::add(DataParseElement *element)
{
  if(element->opening(YARS_STRING_SIMULATOR))
  {
    element->set(YARS_STRING_FREQUENCY, _simulatorFrequency);
  }


  if(element->opening(YARS_STRING_SOLVER))
  {
    element->set(YARS_STRING_ITERATIONS, _solverIterations);
  }

  if(element->opening(YARS_STRING_CONTROL))
  {
    __processControlParameters(element);
    _controlType = CONTROL_TYPE_SIMULATOR;
  }

  if(element->closing(YARS_STRING_SIMULATOR))
  {

    if(_simulatorFrequency == 0) _simulatorFrequency = DEFAULT_SIMULATION_FREQUENCY;
    _stepSize = 1.0 / ((double) _simulatorFrequency);

    current = parent;
  }
}


void DataSimulator::__processControlParameters(DataParseElement *element)
{
  element->set(YARS_STRING_ITERATIONS, _iterations);
  element->set(YARS_STRING_AUTO_RESET, _autoReset);
  element->set(YARS_STRING_SEED,       _randomSeed);
  if(element->attribute(YARS_STRING_SEED) != NULL) _useRandomSeed = true;
}

void DataSimulator::createXsd(XsdSpecification *spec)
{
  XsdSequence *simulatorDefinition = new XsdSequence(YARS_STRING_SIMULATOR_DEFINITION);
  simulatorDefinition->add(NA(YARS_STRING_FREQUENCY, YARS_STRING_POSITIVE_NON_ZERO_INTEGER, true));
  spec->add(simulatorDefinition);

  XsdElement *solverDefinition = NE(YARS_STRING_SOLVER, "", 0, 1);
  solverDefinition->add(NA(YARS_STRING_ITERATIONS, YARS_STRING_POSITIVE_NON_ZERO_INTEGER, true));
  simulatorDefinition->add(solverDefinition);

  XsdElement *control = NE(YARS_STRING_CONTROL, "", 0, 1);
  control->add(NA(YARS_STRING_ITERATIONS, YARS_STRING_XSD_INTEGER, false));
  control->add(NA(YARS_STRING_AUTO_RESET, YARS_STRING_XSD_INTEGER, false));
  control->add(NA(YARS_STRING_SEED,       YARS_STRING_XSD_INTEGER, false));
  // control->add(NA(YARS_STRING_COUNT,      YARS_STRING_COUNT_DEFINITION, false));
  simulatorDefinition->add(control);

  XsdEnumeration *controlCount = new XsdEnumeration(YARS_STRING_COUNT_DEFINITION,
      YARS_STRING_XSD_STRING);
  controlCount->add(YARS_STRING_BEHAVIOUR);
  controlCount->add(YARS_STRING_SIMULATOR);
  spec->add(controlCount);
}

double DataSimulator::stepSize()
{
  return _stepSize;
}

void DataSimulator::setSimulatorFrequency(int frequency)
{
  _simulatorFrequency = frequency;
}

void DataSimulator::setStepSize(double stepSize)
{
  _stepSize = stepSize;
}

DataSimulator* DataSimulator::copy()
{
  DataSimulator *copy = new DataSimulator(NULL);
  copy->_simulatorFrequency = _simulatorFrequency;
  copy->_iterations         = _iterations;
  copy->_autoReset          = _autoReset;
  copy->_stepSize           = _stepSize;
  copy->_controlType        = _controlType;
  copy->_randomSeed         = _randomSeed;
  copy->_useRandomSeed      = _useRandomSeed;
  copy->_solverIterations   = _solverIterations;
  return copy;
}

int DataSimulator::count()
{
  return _controlType;
}

void DataSimulator::setRandomSeed(int seed)
{
  _randomSeed = seed;
  _useRandomSeed = true;
}

int  DataSimulator::getRandomSeed()
{
  return _randomSeed;
}

bool DataSimulator::useRandomSeed()
{
  return _useRandomSeed;
}

int DataSimulator::getSolverIterations()
{
  return _solverIterations;
}
