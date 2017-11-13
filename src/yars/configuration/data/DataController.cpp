#include <yars/configuration/data/DataController.h>
#include <yars/configuration/data/DataActuators.h>

#include <yars/util/Directories.h>
#include <yars/util/YarsErrorHandler.h>

#include <yars/configuration/YarsConfiguration.h>

#include <yars/defines/defaults.h>

# define YARS_STRING_MODULE    (char*)"module"
# define YARS_STRING_FREQUENCY (char*)"frequency"
# define YARS_STRING_POSITIVE_NON_ZERO_INTEGER     (char*)"positive_non_zero_integer" DIVIDER DEFINITION

DataController::DataController(DataNode *parent)
  : DataNode(parent)
{
  pthread_mutexattr_t pthread_mutexattr_default;
  pthread_mutexattr_init(&pthread_mutexattr_default);
  pthread_mutex_init(&_osdMutex, &pthread_mutexattr_default);

  _frequency = 0;
}

void DataController::applySimulatorFrequency(int simFreq)
{
  if(simFreq % _frequency != 0)
  {
    YarsErrorHandler::push("Simulator frequency must be a multiple of the behaviour frequency.");
  }
  _frequency = simFreq / _frequency;
}

void DataController::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_CONTROLLER))
  {
    string n = _name;
    Directories *dir = new Directories();
    if(!dir->getController(&n, _name))
    {
      YarsErrorHandler::push("Controller '%s' not found", _name.c_str());
    }
    _name   = n;

    if(_frequency == 0) _frequency = DEFAULT_CONTROLLER_FREQUENCY;
    current = parent;

  }
  if(element->opening(YARS_STRING_CONTROLLER))
  {
    element->set(YARS_STRING_MODULE,    _module);
    element->set(YARS_STRING_FREQUENCY, _frequency);
    _name = _module;
  }
  if(element->opening(YARS_STRING_PARAMETER))
  {
    DataParameter *parameter = new DataParameter(this);
    parameter->add(element);
    current = parameter;
    push_back(parameter);
  }
}

void DataController::createXsd(XsdSpecification *spec)
{
  XsdSequence *controller = new XsdSequence(YARS_STRING_CONTROLLER_DEFINITION);
  controller->add(NA(YARS_STRING_MODULE,    YARS_STRING_XSD_STRING,           true));
  controller->add(NA(YARS_STRING_FREQUENCY,
        YARS_STRING_POSITIVE_NON_ZERO_INTEGER, true));
  controller->add(NE(YARS_STRING_PARAMETER, YARS_STRING_PARAMETER_DEFINITION, 0));
  spec->add(controller);

  XsdSequence *parameterDefinition = new XsdSequence(YARS_STRING_PARAMETER_DEFINITION);
  parameterDefinition->add(NA(YARS_STRING_NAME,  YARS_STRING_XSD_STRING, true));
  parameterDefinition->add(NA(YARS_STRING_VALUE, YARS_STRING_XSD_STRING, true));
  spec->add(parameterDefinition);
}

string DataController::module()
{
  return _module;
}

void DataController::setModule(string module)
{
  _module = module;
}

DataController* DataController::copy()
{
  DataController *copy = new DataController(NULL);
  copy->_module    = _module;
  copy->_name      = _name;
  copy->_frequency = _frequency;
  for(std::vector<DataParameter*>::iterator i = begin(); i != end(); i++)
  {
    copy->push_back((*i)->copy());
  }
  return copy;
}

void DataController::addLoggable(string name, double *value)
{
  _reals.insert(std::make_pair(name, value));
}

void DataController::addLoggable(string name, int *value)
{
  _ints.insert(std::make_pair(name, value));
}

void DataController::addLoggable(string name, ::Matrix *value)
{
  _matrices.insert(std::make_pair(name, value));
}

void DataController::addLoggable(string name, std::vector<double> *vector)
{
  _vectors.insert(std::make_pair(name, vector));
}


bool DataController::getLoggable(string name, double &real)
{
  for(map<string, double*>::iterator i = _reals.begin(); i != _reals.end(); i++)
  {
    if(i->first == name)
    {
      real = *(i->second);
      return true;
    }
  }
  return false;
}

bool DataController::getLoggable(string name, int &integer)
{
  for(map<string, int*>::iterator i = _ints.begin(); i != _ints.end(); i++)
  {
    if(i->first == name)
    {
      integer = *(i->second);
      return true;
    }
  }
  return false;
}

bool DataController::getLoggable(string name, ::Matrix &matrix)
{
  for(map<string, Matrix*>::iterator m = _matrices.begin(); m != _matrices.end(); m++)
  {
    if(m->first == name)
    {
      matrix = *(m->second);
      return true;
    }
  }
  return false;
}

bool DataController::getLoggable(string name, std::vector<double> &vec)
{
  for(map<string, std::vector<double>* >::iterator i = _vectors.begin(); i != _vectors.end(); i++)
  {
    if(i->first == name)
    {
      vec = *(i->second);
      return true;
    }
  }
  return false;
}

string DataController::name()
{
  return _name;
}


void DataController::resetTo(const DataController *other)
{
  for(int i = 0; i < (int)size(); i++)
  {
    at(i)->resetTo((*other)[i]);
  }
}

void DataController::clearOsd()
{
  _osd.clear();
}

std::vector<string>::const_iterator DataController::s_begin()
{
  return _osd.begin();
}

std::vector<string>::const_iterator DataController::s_end()
{
  return _osd.end();
}

void DataController::addLine(string s)
{
  if(_osd.size() > 0) _osd.push_back("\n");
  _osd.push_back(s);
}

DataActuator* DataController::actuator(string name)
{
  for(DataActuators::iterator a = _actuators.begin(); a != _actuators.end(); a++)
  {
    if((*a)->name() == name) return (*a);
  }
  return NULL;
}

void DataController::addActuator(DataActuator *actuator)
{
  _actuators.push_back(actuator);
}

void DataController::lockOSD()
{
  pthread_mutex_lock(&_osdMutex);
}

void DataController::unlockOSD()
{
  pthread_mutex_unlock(&_osdMutex);
}

int DataController::frequency()
{
  return _frequency;
}
