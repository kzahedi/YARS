#include <yars/configuration/data/DataRobotSimulationDescription.h>

#include <yars/configuration/data/XmlChangeLog.h>

#include <yars/util/stl_macros.h>
#include <yars/util/StringTokeniser.h>

#include <cstdlib>

# define YARS_STRING_VERSION_REGULAR_EXPRESSION (char*)"[0-9]+.[0-9]+.[0-9]+"
# define YARS_STRING_VERSION_DEFINITION         (char*)"version" DIVIDER DEFINITION


DataRobotSimulationDescription::DataRobotSimulationDescription(DataNode*)
  : DataNode(NULL)
{

#include <yars/defines/version.h> // do not move from here

  _simulator   = NULL;
  _screens     = NULL;
  _macros      = NULL;
  _robots    = NULL;
  _environment = NULL;
  _signals     = NULL;
  _traces      = NULL;
  _logging     = NULL;
  _geoms       = new DataObjects();
  _sensors     = new DataSensors();
  _actuators   = new DataActuators();
  _controllers = new DataControllers();

  current      = this;
  parent       = NULL;

}

DataRobotSimulationDescription::~DataRobotSimulationDescription()
{
  if(_screens     != NULL) delete _screens;
  if(_robots      != NULL) delete _robots;
  if(_macros      != NULL) delete _macros;
  if(_environment != NULL) delete _environment;
  if(_signals     != NULL) delete _signals;
  if(_simulator   != NULL) delete _simulator;
  if(_traces      != NULL) delete _traces;
  if(_logging     != NULL) delete _logging;
  delete _geoms;
  delete _sensors;
  delete _actuators;
  delete _controllers;
}

Version DataRobotSimulationDescription::version()
{
  return _version;
}

void DataRobotSimulationDescription::setVersion(Version version)
{
  _version = version;
}

string DataRobotSimulationDescription::comment()
{
  return _comment;
}

DataSimulator* DataRobotSimulationDescription::simulator()
{
  return _simulator;
}

DataScreens* DataRobotSimulationDescription::screens()
{
  return _screens;
}

DataMacros* DataRobotSimulationDescription::macros()
{
  return _macros;
}

DataRobots* DataRobotSimulationDescription::robots()
{
  return _robots;
}

DataEnvironment* DataRobotSimulationDescription::environment()
{
  return _environment;
}

DataLogging* DataRobotSimulationDescription::logging()
{
  return _logging;
}

DataTraces* DataRobotSimulationDescription::traces()
{
  return _traces;
}

DataSignals* DataRobotSimulationDescription::dataSignals()
{
  return _signals;
}

void DataRobotSimulationDescription::add(DataParseElement *element)
{
  if(current == NULL) current = this;
  if(current == this)
  {
    __getChild(element);
  }
  else
  {
    current->add(element);
  }
}

void DataRobotSimulationDescription::__getChild(DataParseElement *element)
{
  if(element->opening(YARS_STRING_SIMULATOR))
  {
    _simulator = new DataSimulator(this);
    current    = _simulator;
    _simulator->add(element);
    return;
  }

  if(element->opening(YARS_STRING_SCREEN_CONFIG))
  {
    _screens = new DataScreens(this);
    current  = _screens;
    _screens->add(element);
    return;
  }

  if(element->opening(YARS_STRING_MACROS))
  {
    _macros = new DataMacros(this);
    current = _macros;
    _macros->add(element);
  }

  if(element->opening(YARS_STRING_ENVIRONMENT))
  {
    _environment = new DataEnvironment(this);
    current       = _environment;
    _environment->add(element);
    _environment->setMacros(_macros);
    return;
  }

  if(element->opening(YARS_STRING_ROBOTS))
  {
    _robots = new DataRobots(this);
    current   = _robots;
    _robots->add(element);
    _robots->setMacros(_macros);
    return;
  }

  if(element->opening(YARS_STRING_SIGNALS_LIST))
  {
    _signals = new DataSignals(this);
    current  = _signals;
    _signals->add(element);
    return;
  }

  if(element->opening(YARS_STRING_ROSIML))
  {
    string v;
    element->set(YARS_STRING_VERSION, v);
    std::vector<string> vs = StringTokeniser::tokenise(v, ".");
    _version.major = atoi(vs[0].c_str());
    _version.minor = atoi(vs[1].c_str());
    _version.patch = atoi(vs[2].c_str());
    if(_version > XmlChangeLog::version())
    {
      YarsErrorHandler *e = YarsErrorHandler::instance();
      (*e).precision(1);
      (*e).setf(ios::fixed,ios::floatfield);
      (*e) << "XML version mismatch. The given XML is of version ";
      (*e) << _version << " but this yars binary only supports XML version ";
      (*e) << XmlChangeLog::version() << endl;
      (*e).unsetf(ios::floatfield);
      YarsErrorHandler::push();
      exit(-1);
    }
    if(_version < XmlChangeLog::version() && _version < XmlChangeLog::last_crucial_change())
    {
      YarsErrorHandler *e = YarsErrorHandler::instance();
      (*e).precision(1);
      (*e).setf(ios::fixed,ios::floatfield);
      (*e) << "XML version mismatch" << endl;
      (*e) << "Showing differences from your XML’s version ";
      (*e) << _version << " to the current version ";
      (*e) << XmlChangeLog::version() << ":" << endl;
      (*e) << XmlChangeLog::changes(_version);
      (*e).unsetf(ios::floatfield);
      YarsErrorHandler::push();
      exit(-1);
    }
    else if(_version < XmlChangeLog::version())
    {
      YarsErrorHandler *e = YarsErrorHandler::instance();
      cout.precision(1);
      cout.setf(ios::fixed,ios::floatfield);
      cout << "Non-critical XML version mismatch" << endl;
      cout << "Showing differences from your XML’s version ";
      cout << _version << " to the current version ";
      cout << XmlChangeLog::version() << ":" << endl;
      cout << XmlChangeLog::changes(_version);
      cout.unsetf(ios::floatfield);
    }
    return;
  }

  if(element->opening(YARS_STRING_TRACES))
  {
    _traces = new DataTraces(this);
    current = _traces;
    _traces->add(element);
  }

  if(element->opening(YARS_STRING_LOGGING))
  {
    _logging = new DataLogging(this);
    current = _logging;
    _logging->add(element);
  }

  if(element->closing(YARS_STRING_ROSIML))
  {
    __checkUniqness();
    __updateGeomsContainer();
    __updateSensorsContainer();
    __updateActuatorsContainer();
    __updateControllersContainer();
    __applySimulatorFrequencyToControllers();
    __updateFollowables();
    __updateTraces();
    __checkLoggingDefinitions();
    __convertForces();
  }
}


void DataRobotSimulationDescription::__applySimulatorFrequencyToControllers()
{
  _controllers->clear();
  if(_robots != NULL)
  {
    for(std::vector<DataRobot*>::iterator m = _robots->begin();
        m != _robots->end(); m++)
    {
      if((*m)->controller() != NULL)
      {
        (*m)->controller()->applySimulatorFrequency(_simulator->simulatorFrequency());
      }
    }
  }
}


void DataRobotSimulationDescription::__checkUniqness()
{
  std::vector<string> allNames;

  if(_robots != NULL)
  {
    for(std::vector<DataRobot*>::iterator m = _robots->begin(); m != _robots->end(); m++)
    {
      allNames.push_back((*m)->name());
      // objects
      for(DataObjects::iterator g = (*m)->g_begin(); g != (*m)->g_end(); g++)
      {
        allNames.push_back((*g)->name());
      }
      // sensors
      for(std::vector<DataSensor*>::iterator s = (*m)->s_begin(); s != (*m)->s_end(); s++)
      {
        allNames.push_back((*s)->name());
      }
      // actuators
      for(std::vector<DataActuator*>::iterator a = (*m)->a_begin(); a != (*m)->a_end(); a++)
      {
        allNames.push_back((*a)->name());
      }
    }
  }
  if(_environment != NULL)
  {
    // for(std::vector<DataEnvironment*>::iterator e = _environments->begin(); e != _environments->end(); e++)
    // {
    //   allNames.push_back((*e)->name());
      // objects
      for(DataObjects::iterator o = _environment->g_begin(); o != _environment->g_end(); o++)
      {
        allNames.push_back((*o)->name());
      }
    // }
  }

  if(allNames.size() > 0)
  {
    sort(allNames.begin(), allNames.end());
    std::vector<string> duplicates;
    for(int i = 0; i < (int)allNames.size() - 1; i++)
    {
      if(allNames[i] == allNames[i+1])
      {
        if(duplicates.size() == 0)
        {
          duplicates.push_back(allNames[i]);
        }
        else if(allNames[i] != duplicates[duplicates.size()-1])
        {
          duplicates.push_back(allNames[i]);
        }
      }
    }
    if(duplicates.size() > 0)
    {
      cout << "Duplicate names found:" << endl;
      for(std::vector<string>::iterator s = duplicates.begin(); s != duplicates.end(); s++)
      {
        cout << "  \"" << *s << "\"" << endl;
      }
      cout << "Quitting yars." << endl;
      exit(-1);
    }
  }
}

void DataRobotSimulationDescription::createXsd(XsdSpecification *spec)
{
  XsdSequence *_root = new XsdSequence(YARS_STRING_ROSIML);
  _root->add(NA(YARS_STRING_VERSION,       YARS_STRING_VERSION_DEFINITION,       true));
  _root->add(NE(YARS_STRING_COMMENT,       YARS_STRING_XSD_STRING,               0, 1));
  _root->add(NE(YARS_STRING_SIMULATOR,     YARS_STRING_SIMULATOR_DEFINITION,     1, 1));
  _root->add(NE(YARS_STRING_SCREEN_CONFIG, YARS_STRING_SCREEN_CONFIG_DEFINITION, 0, 1));
  _root->add(NE(YARS_STRING_MACROS,        YARS_STRING_MACROS_DEFINITION,        0, 1));
  //_root->add(NE(YARS_STRING_ENVIRONMENTS,  YARS_STRING_ENVIRONMENTS_DEFINITION,  0, 1));
  _root->add(NE(YARS_STRING_ENVIRONMENT,   YARS_STRING_ENVIRONMENT_DEFINITION,  0, 1));
  _root->add(NE(YARS_STRING_ROBOTS,      YARS_STRING_ROBOTS_DEFINITION,      0, 1));
  // _root->add(NE(YARS_STRING_SIGNALS_LIST,  YARS_STRING_SIGNALS_LIST_DEFINITION,  0, 1));
  _root->add(NE(YARS_STRING_TRACES,        YARS_STRING_TRACES_DEFINITION,        0, 1));
  _root->add(NE(YARS_STRING_LOGGING,       YARS_STRING_LOGGING_DEFINITION,       0, 1));
  _root->setComment("This is the entry point to the rosiml description");
  spec->setRoot(_root);

  XsdRegularExpression *versionDefinition =
    new XsdRegularExpression(YARS_STRING_VERSION_DEFINITION,
        YARS_STRING_XSD_STRING,
        YARS_STRING_VERSION_REGULAR_EXPRESSION);
  spec->add(versionDefinition);


  DataSimulator::createXsd(spec);
  DataMacros::createXsd(spec);
  DataRobots::createXsd(spec);
  DataEnvironment::createXsd(spec);
  DataScreens::createXsd(spec);
  DataSignals::createXsd(spec);
  DataConstraints::createXsd(spec);
  DataLogging::createXsd(spec);
  DataTraces::createXsd(spec);

  // DataEnvironments::createXsd(spec);
}

void DataRobotSimulationDescription::clear()
{
  if(_screens     != NULL) delete _screens;
  if(_robots    != NULL) delete _robots;
  if(_macros      != NULL) delete _macros;
  if(_environment != NULL) delete _environment;
  if(_signals     != NULL) delete _signals;
  if(_simulator   != NULL) delete _simulator;
  if(_logging     != NULL) delete _logging;
  _screens     = NULL;
  _robots    = NULL;
  _macros      = NULL;
  _environment = NULL;
  _signals     = NULL;
  _simulator   = NULL;
  _logging     = NULL;
  current = this;
}


DataRobotSimulationDescription* DataRobotSimulationDescription::copy()
{
  DataRobotSimulationDescription *copy = new DataRobotSimulationDescription();
  copy->_version      = _version;
  if(_simulator   != NULL) copy->_simulator   = _simulator->copy();
  if(_screens     != NULL) copy->_screens     = _screens->copy();
  if(_macros      != NULL) copy->_macros      = _macros->copy();
  if(_robots    != NULL) copy->_robots    = _robots->copy();
  if(_environment != NULL) copy->_environment = _environment->copy();
  if(_signals     != NULL) copy->_signals     = _signals->copy();
  if(_traces      != NULL) copy->_traces      = _traces->copy();
  if(_logging     != NULL) copy->_logging     = _logging->copy();
  copy->__updateGeomsContainer();
  copy->__updateSensorsContainer();
  copy->__updateActuatorsContainer();
  copy->__updateControllersContainer();
  copy->__updateFollowables();
  copy->__updateTraces();
  copy->__checkLoggingDefinitions();
  return copy;
}

void DataRobotSimulationDescription::__updateFollowables()
{
  if(_screens == NULL) return;
  DataFollowables *followables = _screens->followables();
  if(followables         == NULL) return;
  if(followables->size() == 0)    return;
  for(std::vector<string>::iterator i = followables->begin(); i != followables->end(); i++)
  {
    for(DataObjects::iterator o = _geoms->begin(); o != _geoms->end(); o++)
    {
      if(*i == (*o)->name())
      {
        followables->add(*o);
      }
    }
  }
}

void DataRobotSimulationDescription::__updateTraces()
{
  if(_traces == NULL) return;
  if(_traces->l_size() != 0)
    for(std::vector<DataTraceLine*>::iterator l = _traces->l_begin(); l != _traces->l_end(); l++)
    {
      bool found = false;
      for(DataObjects::iterator o = _geoms->begin(); o != _geoms->end(); o++)
      {
        if((*l)->target() == (*o)->name())
        {
          (*l)->setTarget(*o);
          found = true;
        }
      }
      if(!found)
      {
        YarsErrorHandler *e = YarsErrorHandler::instance();
        (*e) << "Could not find the target " << (*l)->target() << " for trace line";
        YarsErrorHandler::push();
      }
    }

  if(_traces->p_size() != 0)
    for(std::vector<DataTracePoint*>::iterator p = _traces->p_begin(); p != _traces->p_end(); p++)
      for(DataObjects::iterator o = _geoms->begin(); o != _geoms->end(); o++)
        if((*p)->target() == (*o)->name())
          (*p)->setTarget(*o);
}

void DataRobotSimulationDescription::__updateGeomsContainer()
{
  _geoms->clear();
  if(_robots != NULL)
  {
    for(std::vector<DataRobot*>::iterator m = _robots->begin(); m != _robots->end(); m++)
    {
      for(DataObjects::iterator g = (*m)->g_begin(); g != (*m)->g_end(); g++)
      {
        _geoms->add(*g);
      }
    }
  }
  if(_environment != NULL)
  {
    // for(std::vector<DataEnvironment*>::iterator e = _environments->begin(); e != _environments->end(); e++)
    // {
    for(DataObjects::iterator o = _environment->g_begin(); o != _environment->g_end(); o++)
    {
      _geoms->add(*o);
    }
    // }
  }
}

void DataRobotSimulationDescription::__updateSensorsContainer()
{
  _sensors->clear();
  if(_robots != NULL)
  {
    for(std::vector<DataRobot*>::iterator m = _robots->begin(); m != _robots->end(); m++)
    {
      for(DataSensors::iterator s = (*m)->s_begin(); s != (*m)->s_end(); s++)
      {
        _sensors->add(*s);
      }
    }
  }
}

void DataRobotSimulationDescription::__updateActuatorsContainer()
{
  _actuators->clear();
  if(_robots != NULL)
  {
    for(std::vector<DataRobot*>::iterator m = _robots->begin(); m != _robots->end(); m++)
    {
      for(DataActuators::iterator a = (*m)->a_begin(); a != (*m)->a_end(); a++)
      {
        _actuators->add(*a);
      }
    }
  }
}

void DataRobotSimulationDescription::__updateControllersContainer()
{
  _controllers->clear();
  if(_robots != NULL)
  {
    for(std::vector<DataRobot*>::iterator m = _robots->begin(); m != _robots->end(); m++)
    {
      _controllers->add((*m)->controller());
    }
  }
}

void DataRobotSimulationDescription::__checkLoggingDefinitions()
{
  if(_logging == NULL) return;
  _logging->checkGeoms(_geoms);
  _logging->checkSensors(_sensors);
  _logging->checkActuators(_actuators);
  _logging->checkControllers(_controllers);
  _logging->finish();
}


void DataRobotSimulationDescription::resetTo(const DataRobotSimulationDescription *rsd)
{
  _version = rsd->_version;

  _robots->resetTo(rsd->_robots);
  _environment->resetTo(rsd->_environment);
  if(_traces != NULL) _traces->resetTo(rsd->_traces);

  // _simulator << rsd._simulator; // not required
  // _screen    << rsd._screens;   // not required
  // _macros    << rsd._macros;    // not required
  // if(_environment != NULL) copy->_environment = _environment->copy();
  // if(_signals     != NULL) copy->_signals     = _signals->copy();
  // if(_traces      != NULL) copy->_traces      = _traces->copy();
  // if(_logging     != NULL) copy->_logging     = _logging->copy();
}

void DataRobotSimulationDescription::__convertForces()
{
  DataHingeActuator *ha   = NULL;
  DataSliderActuator *sa  = NULL;
  DataGenericActuator *ga = NULL;
  for(DataActuators::iterator i = _actuators->begin(); i != _actuators->end(); i++)
  {
    switch((*i)->type())
    {
      case DATA_ACTUATOR_HINGE:
        ha = (DataHingeActuator*)(*i);
        ha->setForce(ha->force() * ((yReal) _simulator->simulatorFrequency()));
        // cout << "Set data hinge force to " << ha->force() << endl;
        break;
      case DATA_ACTUATOR_SLIDER:
        sa = (DataSliderActuator*)(*i);
        sa->setForce(sa->force() * ((yReal) _simulator->simulatorFrequency()));
        // cout << "Set data slider force to " << ha->force() << endl;
        break;
      case DATA_ACTUATOR_UNIVERSAL:
        ga = (DataGenericActuator*)(*i);
        for(int i = 0; i < ga->dimension(); i++)
        {
          ga->setForce(i, ga->force(i) * ((yReal) _simulator->simulatorFrequency()));
          // cout << "Set data generic force to " << i << " : " << ga->force(i) << endl;
        }
        break;
    }
  }
}
