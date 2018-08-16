#include "DataRobot.h"
#include "DataPoseFactory.h"

# define YARS_STRING_POSE                  (char*)"pose"
# define YARS_STRING_NAME                  (char*)"name"
# define YARS_STRING_POSE_DEFINITION       (char*)"pose_definition"
# define YARS_STRING_BODY_DEFINITION       (char*)"body_definition"
# define YARS_STRING_SELF_COLLIDE          (char*)"selfCollide"
# define YARS_STRING_TRUE_FALSE_DEFINITION (char*)"true_false_definition"

DataRobot::DataRobot(DataNode *parent)
  : DataNode(parent)
{
  _controller          = NULL;
  _selfCollide         = false;
  _processingSensors   = false;
  _processingActuators = false;
}

DataRobot::~DataRobot()
{
  for(DataObjects::iterator i = _objects.begin(); i != _objects.end(); i++)
  {
    delete (*i);
  }
  for(std::vector<DataSensor*>::iterator i = _sensors.begin(); i != _sensors.end(); i++)
  {
    delete (*i);
  }
  for(std::vector<DataActuator*>::iterator i = _actuators.begin(); i != _actuators.end(); i++)
  {
    delete (*i);
  }
  for(std::vector<DataMacroInstance*>::iterator i = _macros.begin(); i != _macros.end(); i++)
  {
    delete (*i);
  }
  _objects.clear();
  _sensors.clear();
  _actuators.clear();
  _macros.clear();
  if(_controller != NULL) delete _controller;
}

string DataRobot::name()
{
  return _name;
}

Pose DataRobot::pose()
{
  return _pose;
}

DataObjects::iterator DataRobot::o_begin()
{
  return _objects.begin();
}

DataObjects::iterator DataRobot::o_end()
{
  return _objects.end();
}

int DataRobot::o_size()
{
  return (int)_objects.size();
}

DataObjects::iterator DataRobot::g_begin()
{
  return _geoms.begin();
}

DataObjects::iterator DataRobot::g_end()
{
  return _geoms.end();
}

int DataRobot::g_size()
{
  return (int)_geoms.size();
}

std::vector<DataSensor*>::iterator DataRobot::s_begin()
{
  return _sensors.begin();
}

std::vector<DataSensor*>::iterator DataRobot::s_end()
{
  return _sensors.end();
}

int DataRobot::s_size()
{
  return (int)_sensors.size();
}

std::vector<DataActuator*>::iterator DataRobot::a_begin()
{
  return _actuators.begin();
}

std::vector<DataActuator*>::iterator DataRobot::a_end()
{
  return _actuators.end();
}

int DataRobot::a_size()
{
  return (int)_actuators.size();
}

void DataRobot::add(DataParseElement *element)
{
  if(element->closing(YARS_STRING_ROBOT))
  {
    current = parent;
    // __assignSensorsToObjects(); // only needs to be called after copying
    __applyMacros();
    __gatherGeoms();
    __applyPose();
    __setActuatorsInController();
  }

  if(element->opening(YARS_STRING_ROBOT))
  {
    element->set(YARS_STRING_NAME, _name);
  }

  if(element->opening(YARS_STRING_BODY))
  {
    element->set(YARS_STRING_SELF_COLLIDE, _selfCollide);
  }

  if(element->opening(YARS_STRING_POSE))
  {
    DataPoseFactory::set(_pose, element);
  }

  if(element->opening(YARS_STRING_MACRO))
  {
    DataMacroInstance *macroInstance = new DataMacroInstance(this, _macrosDefinitions);
    current = macroInstance;
    macroInstance->add(element);
    _macros.push_back(macroInstance);
  }

  DataObject *object = DataObjectFactory::object(element, this);
  if(object != NULL)
  {
    current = object;
    _objects.push_back(object);
  }

  if(element->opening(YARS_STRING_SENSOR_LIST))
  {
    _processingSensors = true;
  }
  if(element->closing(YARS_STRING_SENSOR_LIST))
  {
    _processingSensors = false;
  }
  if(element->opening(YARS_STRING_ACTUATOR_LIST))
  {
    _processingActuators = true;
  }
  if(element->closing(YARS_STRING_ACTUATOR_LIST))
  {
    _processingActuators = false;
  }

  if(_processingSensors)
  {
    DataSensor *sensor = DataSensorFactory::sensor(element, this);
    if(sensor != NULL)
    {
      current = sensor;
      _sensors.push_back(sensor);
    }
  }

  if(_processingActuators)
  {
    DataActuator *actuator = DataActuatorFactory::actuator(element, this);
    if(actuator != NULL)
    {
      current = actuator;
      _actuators.push_back(actuator);
    }
  }

  if(element->opening(YARS_STRING_CONTROLLER))
  {
    _controller = new DataController(this);
    current = _controller;
    _controller->add(element);
  }
}

void DataRobot::setMacros(DataMacros *macros)
{
  _macrosDefinitions = macros;
}

DataMacros* DataRobot::macros()
{
  return _macrosDefinitions;
}

DataController* DataRobot::controller()
{
  return _controller;
}

DataSensor* DataRobot::sensor(int index)
{
  return _sensors[index];
}

DataActuator* DataRobot::actuator(int index)
{
  return _actuators[index];
}

bool DataRobot::selfCollide()
{
  return _selfCollide;
}

void DataRobot::__applyMacros()
{
  for(std::vector<DataMacroInstance*>::iterator i = _macros.begin(); i != _macros.end(); i++)
  {
    for(DataObjects::iterator o = (*i)->begin(); o != (*i)->end(); o++)
    {
      _objects.push_back(*o);
    }
  }
}

void DataRobot::__applyPose()
{
  for(DataObjects::iterator object_ptr = _objects.begin(); object_ptr != _objects.end(); object_ptr++)
  {
    if((*object_ptr)->type() == DATA_OBJECT_COMPOSITE)
    {
      ((DataComposite*)(*object_ptr))->applyOffset(_pose);
    }
    else
    {
      (*object_ptr)->applyOffset(_pose);
    }
  }

  for(std::vector<DataActuator*>::iterator actuator_ptr = _actuators.begin(); actuator_ptr != _actuators.end(); actuator_ptr++)
  {
    for(DataObjects::iterator j = _geoms.begin(); j != _geoms.end(); j++)
    {
      if((*actuator_ptr)->source() == (*j)->name())
      {
        (*actuator_ptr)->applyOffset((*j)->pose());
      }
    }
  }

  // muscle
  for (std::vector<DataActuator *>::iterator a = _actuators.begin(); a != _actuators.end(); a++)
  { 
    if ((*a)->type() == DATA_ACTUATOR_MUSCLE)
    {
      DataMuscleActuator *m = (DataMuscleActuator *)(*a);
      string source = m->source();
      string srcName = m->sourceObject()->name();

      string dstName = m->destinationObject()->name();
      string destination = m->destination();

      DataObject *srcObject = __findObject(source);
      DataObject *dstObject = __findObject(destination);
      DataObject *srcConnector = __findObject(srcName);
      DataObject *dstConnector = __findObject(dstName);

      srcConnector->applyOffset(srcObject->pose());
      dstConnector->applyOffset(dstObject->pose());
    }
  }
}

void DataRobot::createXsd(XsdSpecification *spec)
{
  XsdSequence *robotDefinition = new XsdSequence(YARS_STRING_ROBOT_DEFINITION);
  robotDefinition->add(NA(YARS_STRING_NAME,          YARS_STRING_XSD_STRING,               false));
  robotDefinition->add(NE(YARS_STRING_BODY,          YARS_STRING_BODY_DEFINITION,          1, 1));
  robotDefinition->add(NE(YARS_STRING_POSE,          YARS_STRING_POSE_DEFINITION,          0, 1));
  robotDefinition->add(NE(YARS_STRING_ACTUATOR_LIST, YARS_STRING_ACTUATOR_LIST_DEFINITION, 0, 1));
  robotDefinition->add(NE(YARS_STRING_SENSOR_LIST,   YARS_STRING_SENSOR_LIST_DEFINITION,   0, 1));
  // robotDefinition->add(NE(YARS_STRING_CONSTRAINTS,   YARS_STRING_CONSTRAINTS_DEFINITION,   0, 1));
  robotDefinition->add(NE(YARS_STRING_CONTROLLER,    YARS_STRING_CONTROLLER_DEFINITION,    0, 1));
  spec->add(robotDefinition);

  XsdSequence *bodyDefinition = new XsdSequence(YARS_STRING_BODY_DEFINITION);
  bodyDefinition->add(NA(YARS_STRING_SELF_COLLIDE, YARS_STRING_TRUE_FALSE_DEFINITION, false));
  XsdChoice *objectChoice = new XsdChoice("",              "1", YARS_STRING_XSD_UNBOUNDED);
  objectChoice->add(NE(YARS_STRING_OBJECT_BOX,             YARS_STRING_OBJECT_BOX_DEFINTION,             0));
  objectChoice->add(NE(YARS_STRING_OBJECT_SPHERE,          YARS_STRING_OBJECT_SPHERE_DEFINTION,          0));
  objectChoice->add(NE(YARS_STRING_OBJECT_PLY,             YARS_STRING_OBJECT_PLY_DEFINTION,             0));
  objectChoice->add(NE(YARS_STRING_OBJECT_CYLINDER,        YARS_STRING_OBJECT_CYLINDER_DEFINTION,        0));
  objectChoice->add(NE(YARS_STRING_OBJECT_CAPPED_CYLINDER, YARS_STRING_OBJECT_CAPPED_CYLINDER_DEFINTION, 0));
  objectChoice->add(NE(YARS_STRING_OBJECT_COMPOSITE,       YARS_STRING_OBJECT_COMPOSITE_DEFINITION,      0));
  objectChoice->add(NE(YARS_STRING_OBJECT_MACRO,           YARS_STRING_OBJECT_MACRO_DEFINTION,           0));
  bodyDefinition->add(objectChoice);
  spec->add(bodyDefinition);

  DataObjectFactory::createXsd(spec);
  DataSensorFactory::createXsd(spec);
  DataActuatorFactory::createXsd(spec);
  DataController::createXsd(spec);
  DataMacroInstance::createXsd(spec);
}

DataRobot* DataRobot::copy()
{
  DataRobot *copy = new DataRobot(NULL);
  copy->_pose                = _pose;
  copy->_name                = _name;
  copy->_processingSensors   = _processingSensors;
  copy->_processingActuators = _processingActuators;
  copy->_selfCollide         = _selfCollide;
  if(_controller        != NULL) copy->_controller        = _controller->copy();
  if(_macrosDefinitions != NULL) copy->_macrosDefinitions = _macrosDefinitions->copy();
  for(DataObjects::iterator i = _objects.begin(); i != _objects.end(); i++)
  {
    copy->_objects.push_back((*i)->copy());
  }
  for(std::vector<DataSensor*>::iterator i = _sensors.begin(); i != _sensors.end(); i++)
  {
    copy->_sensors.push_back((*i)->copy());
  }
  for(std::vector<DataActuator*>::iterator i = _actuators.begin(); i != _actuators.end(); i++)
  {
    copy->_actuators.push_back((*i)->copy());
  }
  for(std::vector<DataMacroInstance*>::iterator i = _macros.begin(); i != _macros.end(); i++)
  {
    copy->_macros.push_back((*i)->copy(this));
  }
  copy->__gatherGeoms();
  copy->__assignSensorsToObjects();
  copy->__setMotorVector();
  copy->__setSensorVector();
  copy->__setActuatorsInController();

  return copy;
}

void DataRobot::__setActuatorsInController()
{
  if(_controller != NULL)
  {
    for(DataActuators::iterator a = _actuators.begin(); a != _actuators.end(); a++)
    {
      _controller->addActuator(*a);
    }
  }
}

void DataRobot::__setMotorVector()
{
  int nrOfValues = 0;
  for(std::vector<DataActuator*>::iterator i = _actuators.begin(); i != _actuators.end(); i++)
  {
    for(int j = 0; j < (*i)->dimension(); j++) if((*i)->isActive(j)) nrOfValues++;
  }

  _actuatorValues.resize(nrOfValues);
}

void DataRobot::__setSensorVector()
{
  int nrOfValues = 0;
  for(std::vector<DataSensor*>::iterator i = _sensors.begin(); i != _sensors.end(); i++)
  {
    nrOfValues += (*i)->dimension();
  }
  _sensorValues.resize(nrOfValues);
}

void DataRobot::__gatherGeoms()
{
  for(DataObjects::iterator o = _objects.begin(); o != _objects.end(); o++)
  {
    if((*o)->type() == DATA_OBJECT_COMPOSITE)
    {
      DataComposite *composite = (DataComposite*)(*o);
      for(DataObjects::iterator oo = composite->g_begin(); oo != composite->g_end(); oo++)
      {
        _geoms.push_back(*oo);
      }
    }
    else
    {
      _geoms.push_back(*o);
    }
  }

  for(vector<DataActuator*>::iterator a = _actuators.begin(); a != _actuators.end(); a++)
  {
    if((*a)->type() == DATA_ACTUATOR_MUSCLE)
    {
      DataMuscleActuator *m = (DataMuscleActuator*)(*a);
      DataObject* src = m->sourceObject();
      DataObject* dst = m->destinationObject();
      _geoms.push_back(src);
      _geoms.push_back(dst);
    }
  }
}

void DataRobot::__assignSensorsToObjects()
{
  for(std::vector<DataSensor*>::iterator s = _sensors.begin(); s != _sensors.end(); s++)
  {
    for(DataObjects::iterator g = _geoms.begin(); g != _geoms.end(); g++)
    {
      if((*s)->object() == (*g)->name())
      {
        (*g)->addSensor(*s);
      }
    }
  }
}

std::vector<double>::iterator DataRobot::av_begin()
{
  return _actuatorValues.begin();
}

std::vector<double>::iterator DataRobot::av_end()
{
  return _actuatorValues.end();
}

int DataRobot::av_size()
{
  return _actuatorValues.size();
}

std::vector<double>::iterator DataRobot::sv_begin()
{
  return _sensorValues.begin();
}

std::vector<double>::iterator DataRobot::sv_end()
{
  return _sensorValues.end();
}

int DataRobot::sv_size()
{
  return _sensorValues.size();
}

void DataRobot::updateSensorValues()
{
  int index = 0;
  for(std::vector<DataSensor*>::iterator i = _sensors.begin(); i != _sensors.end(); i++)
  {
    for(int j = 0; j < (*i)->dimension(); j++) _sensorValues[index++] = (*i)->externalValue(j);
  }
}

void DataRobot::updateActuatorValues()
{
  int index = 0;
  for(std::vector<DataActuator*>::iterator i = _actuators.begin(); i != _actuators.end(); i++)
  {
    int activeIndex = 0;
    // cout << "dimension: " << (*i)->dimension() << endl;
    for(int j = 0; j < (*i)->dimension(); j++)
    {
      if((*i)->isActive(j))
      {
        // cout << activeIndex << " set to " << _actuatorValues[index] << endl;
        (*i)->setDesiredValue(activeIndex++, _actuatorValues[index++]);
      }
    }
  }
}

void DataRobot::resetTo(const DataRobot *robot)
{
  _pose                = robot->_pose;
  _name                = robot->_name;
  _selfCollide         = robot->_selfCollide;

  if(_controller != NULL)                       _controller->resetTo(robot->_controller);
  for(int i = 0; i < (int)_objects.size(); i++) _objects[i]->resetTo(robot->_objects[i]);
  for(int i = 0; i < (int)_sensors.size(); i++) _sensors[i]->resetTo(robot->_sensors[i]);
}

DataObject* DataRobot::__findObject(string name)
{
  for(vector<DataObject*>::iterator o = _geoms.begin(); o != _geoms.end(); o++)
  {
    if((*o)->name() == name)
    {
      return *o;
    }
  }
  return NULL;
}